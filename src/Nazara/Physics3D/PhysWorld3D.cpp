// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/PhysWorld3D.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Newton/Newton.h>
#include <cassert>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	PhysWorld3D::PhysWorld3D() :
	m_gravity(Vector3f::Zero()),
	m_maxStepCount(50),
	m_stepSize(0.005f),
	m_timestepAccumulator(0.f)
	{
		m_world = NewtonCreate();
		NewtonWorldSetUserData(m_world, this);

		m_materialIds.emplace("default", NewtonMaterialGetDefaultGroupID(m_world));
	}

	PhysWorld3D::~PhysWorld3D()
	{
		NewtonDestroy(m_world);
	}

	int PhysWorld3D::CreateMaterial(String name)
	{
		NazaraAssert(m_materialIds.find(name) == m_materialIds.end(), "Material \"" + name + "\" already exists");

		int materialId = NewtonMaterialCreateGroupID(m_world);
		m_materialIds.emplace(std::move(name), materialId);

		return materialId;
	}

	void PhysWorld3D::ForEachBodyInAABB(const Boxf& box, const BodyIterator& iterator)
	{
		auto NewtonCallback = [](const NewtonBody* const body, void* const userdata) -> int
		{
			const BodyIterator& bodyIterator = *static_cast<BodyIterator*>(userdata);
			return bodyIterator(*static_cast<RigidBody3D*>(NewtonBodyGetUserData(body)));
		};

		NewtonWorldForEachBodyInAABBDo(m_world, box.GetMinimum(), box.GetMaximum(), NewtonCallback, const_cast<void*>(static_cast<const void*>(&iterator)));
	}

	Vector3f PhysWorld3D::GetGravity() const
	{
		return m_gravity;
	}

	NewtonWorld* PhysWorld3D::GetHandle() const
	{
		return m_world;
	}

	int PhysWorld3D::GetMaterial(const String& name)
	{
		auto it = m_materialIds.find(name);
		NazaraAssert(it != m_materialIds.end(), "Material \"" + name + "\" does not exists");

		return it->second;
	}

	std::size_t PhysWorld3D::GetMaxStepCount() const
	{
		return m_maxStepCount;
	}

	float PhysWorld3D::GetStepSize() const
	{
		return m_stepSize;
	}

	void PhysWorld3D::SetGravity(const Vector3f& gravity)
	{
		m_gravity = gravity;
	}

	void PhysWorld3D::SetMaxStepCount(std::size_t maxStepCount)
	{
		m_maxStepCount = maxStepCount;
	}

	void PhysWorld3D::SetSolverModel(unsigned int model)
	{
		NewtonSetSolverModel(m_world, model);
	}

	void PhysWorld3D::SetStepSize(float stepSize)
	{
		m_stepSize = stepSize;
	}

	void PhysWorld3D::SetMaterialCollisionCallback(int firstMaterial, int secondMaterial, AABBOverlapCallback aabbOverlapCallback, CollisionCallback collisionCallback)
	{
		static_assert(sizeof(UInt64) >= 2 * sizeof(int), "Oops");

		auto callbackPtr = std::make_unique<Callback>();
		callbackPtr->aabbOverlapCallback = std::move(aabbOverlapCallback);
		callbackPtr->collisionCallback = std::move(collisionCallback);

		NewtonMaterialSetCollisionCallback(m_world, firstMaterial, secondMaterial, callbackPtr.get(), (callbackPtr->aabbOverlapCallback) ? OnAABBOverlap : nullptr, (callbackPtr->collisionCallback) ? ProcessContact : nullptr);

		UInt64 firstMaterialId(firstMaterial);
		UInt64 secondMaterialId(secondMaterial);

		UInt64 callbackIndex = firstMaterialId << 32 | secondMaterialId;
		m_callbacks[callbackIndex] = std::move(callbackPtr);
	}

	void PhysWorld3D::SetMaterialDefaultCollidable(int firstMaterial, int secondMaterial, bool collidable)
	{
		NewtonMaterialSetDefaultCollidable(m_world, firstMaterial, secondMaterial, collidable);
	}

	void PhysWorld3D::SetMaterialDefaultElasticity(int firstMaterial, int secondMaterial, float elasticCoef)
	{
		NewtonMaterialSetDefaultElasticity(m_world, firstMaterial, secondMaterial, elasticCoef);
	}

	void PhysWorld3D::SetMaterialDefaultFriction(int firstMaterial, int secondMaterial, float staticFriction, float kineticFriction)
	{
		NewtonMaterialSetDefaultFriction(m_world, firstMaterial, secondMaterial, staticFriction, kineticFriction);
	}

	void PhysWorld3D::SetMaterialDefaultSoftness(int firstMaterial, int secondMaterial, float softness)
	{
		NewtonMaterialSetDefaultSoftness(m_world, firstMaterial, secondMaterial, softness);
	}

	void PhysWorld3D::SetMaterialSurfaceThickness(int firstMaterial, int secondMaterial, float thickness)
	{
		NewtonMaterialSetSurfaceThickness(m_world, firstMaterial, secondMaterial, thickness);
	}

	void PhysWorld3D::Step(float timestep)
	{
		m_timestepAccumulator += timestep;

		std::size_t stepCount = 0;
		while (m_timestepAccumulator >= m_stepSize && stepCount < m_maxStepCount)
		{
			NewtonUpdate(m_world, m_stepSize);
			m_timestepAccumulator -= m_stepSize;
			stepCount++;
		}
	}

	int PhysWorld3D::OnAABBOverlap(const NewtonMaterial* const material, const NewtonBody* const body0, const NewtonBody* const body1, int threadIndex)
	{
		RigidBody3D* bodyA = static_cast<RigidBody3D*>(NewtonBodyGetUserData(body0));
		RigidBody3D* bodyB = static_cast<RigidBody3D*>(NewtonBodyGetUserData(body1));
		assert(bodyA && bodyB);

		Callback* callbackData = static_cast<Callback*>(NewtonMaterialGetMaterialPairUserData(material));
		assert(callbackData);
		assert(callbackData->aabbOverlapCallback);

		return callbackData->aabbOverlapCallback(*bodyA, *bodyB);
	}

	void PhysWorld3D::ProcessContact(const NewtonJoint* const contactJoint, float timestep, int threadIndex)
	{
		RigidBody3D* bodyA = static_cast<RigidBody3D*>(NewtonBodyGetUserData(NewtonJointGetBody0(contactJoint)));
		RigidBody3D* bodyB = static_cast<RigidBody3D*>(NewtonBodyGetUserData(NewtonJointGetBody1(contactJoint)));
		assert(bodyA && bodyB);

		using ContactJoint = void*;

		// Query all joints first, to prevent removing a joint from the list while iterating on it
		StackVector<ContactJoint> contacts = NazaraStackVector(ContactJoint, NewtonContactJointGetContactCount(contactJoint));
		for (ContactJoint contact = NewtonContactJointGetFirstContact(contactJoint); contact; contact = NewtonContactJointGetNextContact(contactJoint, contact))
			contacts.push_back(contact);

		for (ContactJoint contact : contacts)
		{
			NewtonMaterial* material = NewtonContactGetMaterial(contact);
			Callback* callbackData = static_cast<Callback*>(NewtonMaterialGetMaterialPairUserData(material));
			assert(callbackData);
			assert(callbackData->collisionCallback);

			if (!callbackData->collisionCallback(*bodyA, *bodyB))
				NewtonContactJointRemoveContact(contactJoint, contact);
		}
	}
}
