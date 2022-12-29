// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/PhysWorld3D.hpp>
#include <Nazara/Utils/StackVector.hpp>
#include <newton/Newton.h>
#include <cassert>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	PhysWorld3D::PhysWorld3D() :
	m_maxStepCount(50),
	m_gravity(Vector3f::Zero()),
	m_stepSize(Time::TickDuration(120)),
	m_timestepAccumulator(Time::Zero())
	{
		m_world = NewtonCreate();
		NewtonWorldSetUserData(m_world, this);

		m_materialIds.emplace("default", NewtonMaterialGetDefaultGroupID(m_world));
	}

	PhysWorld3D::PhysWorld3D(PhysWorld3D&& physWorld) noexcept :
	m_callbacks(std::move(physWorld.m_callbacks)),
	m_materialIds(std::move(physWorld.m_materialIds)),
	m_maxStepCount(std::move(physWorld.m_maxStepCount)),
	m_world(std::move(physWorld.m_world)),
	m_gravity(std::move(physWorld.m_gravity)),
	m_stepSize(std::move(physWorld.m_stepSize)),
	m_timestepAccumulator(std::move(physWorld.m_timestepAccumulator))
	{
		NewtonWorldSetUserData(m_world, this);
	}

	PhysWorld3D::~PhysWorld3D()
	{
		if (m_world)
			NewtonDestroy(m_world);
	}

	int PhysWorld3D::CreateMaterial(std::string name)
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

		Vector3f min = box.GetMinimum();
		Vector3f max = box.GetMaximum();
		NewtonWorldForEachBodyInAABBDo(m_world, &min.x, &max.x, NewtonCallback, const_cast<void*>(static_cast<const void*>(&iterator)));
	}

	Vector3f PhysWorld3D::GetGravity() const
	{
		return m_gravity;
	}

	NewtonWorld* PhysWorld3D::GetHandle() const
	{
		return m_world;
	}

	int PhysWorld3D::GetMaterial(const std::string& name)
	{
		auto it = m_materialIds.find(name);
		NazaraAssert(it != m_materialIds.end(), "Material \"" + name + "\" does not exists");

		return it->second;
	}

	std::size_t PhysWorld3D::GetMaxStepCount() const
	{
		return m_maxStepCount;
	}

	Time PhysWorld3D::GetStepSize() const
	{
		return m_stepSize;
	}

	unsigned int PhysWorld3D::GetThreadCount() const
	{
		return NewtonGetThreadsCount(m_world);
	}

	void PhysWorld3D::SetGravity(const Vector3f& gravity)
	{
		m_gravity = gravity;
	}

	void PhysWorld3D::SetMaxStepCount(std::size_t maxStepCount)
	{
		m_maxStepCount = maxStepCount;
	}

	void PhysWorld3D::SetStepSize(Time stepSize)
	{
		m_stepSize = stepSize;
	}

	void PhysWorld3D::SetThreadCount(unsigned int threadCount)
	{
		NewtonSetThreadsCount(m_world, threadCount);
	}

	void PhysWorld3D::SetMaterialCollisionCallback(int firstMaterial, int secondMaterial, AABBOverlapCallback aabbOverlapCallback, CollisionCallback collisionCallback)
	{
		static_assert(sizeof(UInt64) >= 2 * sizeof(int), "Oops");

		auto callbackPtr = std::make_unique<Callback>();
		callbackPtr->aabbOverlapCallback = std::move(aabbOverlapCallback);
		callbackPtr->collisionCallback = std::move(collisionCallback);

		NewtonMaterialSetCollisionCallback(m_world, firstMaterial, secondMaterial, (callbackPtr->aabbOverlapCallback) ? OnAABBOverlap : nullptr, (callbackPtr->collisionCallback) ? ProcessContact : nullptr);
		NewtonMaterialSetCallbackUserData(m_world, firstMaterial, secondMaterial, callbackPtr.get());

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

	void PhysWorld3D::Step(Time timestep)
	{
		m_timestepAccumulator += timestep;

		std::size_t stepCount = 0;
		float dt = m_stepSize.AsSeconds<float>();

		while (m_timestepAccumulator >= m_stepSize && stepCount < m_maxStepCount)
		{
			NewtonUpdate(m_world, dt);
			m_timestepAccumulator -= m_stepSize;
			stepCount++;
		}
	}

	PhysWorld3D& PhysWorld3D::operator=(PhysWorld3D&& physWorld) noexcept
	{
		if (m_world)
			NewtonDestroy(m_world);

		m_callbacks = std::move(physWorld.m_callbacks);
		m_materialIds = std::move(physWorld.m_materialIds);
		m_maxStepCount = std::move(physWorld.m_maxStepCount);
		m_world = std::move(physWorld.m_world);
		m_gravity = std::move(physWorld.m_gravity);
		m_stepSize = std::move(physWorld.m_stepSize);
		m_timestepAccumulator = std::move(physWorld.m_timestepAccumulator);

		NewtonWorldSetUserData(m_world, this);

		return *this;
	}

	int PhysWorld3D::OnAABBOverlap(const NewtonJoint* const contactJoint, float /*timestep*/, int /*threadIndex*/)
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
				return 0;
		}

		return 1;
	}

	void PhysWorld3D::ProcessContact(const NewtonJoint* const contactJoint, float /*timestep*/, int /*threadIndex*/)
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
