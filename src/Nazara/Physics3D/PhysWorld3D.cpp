// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/PhysWorld3D.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <Newton/Newton.h>
#include <cassert>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	PhysWorld3D::PhysWorld3D() :
	m_gravity(Vector3f::Zero()),
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

	int PhysWorld3D::CreateMaterial(Nz::String name)
	{
		NazaraAssert(m_materialIds.find(name) == m_materialIds.end(), "Material \"" + name + "\" already exists");

		int materialId = NewtonMaterialCreateGroupID(m_world);
		m_materialIds.emplace(std::move(name), materialId);

		return materialId;
	}

	Vector3f PhysWorld3D::GetGravity() const
	{
		return m_gravity;
	}

	NewtonWorld* PhysWorld3D::GetHandle() const
	{
		return m_world;
	}

	int PhysWorld3D::GetMaterial(const Nz::String& name)
	{
		auto it = m_materialIds.find(name);
		NazaraAssert(it != m_materialIds.end(), "Material \"" + name + "\" does not exists");

		return it->second;
	}

	float PhysWorld3D::GetStepSize() const
	{
		return m_stepSize;
	}

	void PhysWorld3D::SetGravity(const Vector3f& gravity)
	{
		m_gravity = gravity;
	}

	void PhysWorld3D::SetSolverModel(unsigned int model)
	{
		NewtonSetSolverModel(m_world, model);
	}

	void PhysWorld3D::SetStepSize(float stepSize)
	{
		m_stepSize = stepSize;
	}

	void PhysWorld3D::SetMaterialCollisionCallback(int firstMaterial, int secondMaterial, CollisionCallback callback)
	{
		static_assert(sizeof(Nz::UInt64) >= 2 * sizeof(int), "Oops");

		auto callbackPtr = std::make_unique<Callback>();
		callbackPtr->collisionCallback = std::move(callback);

		NewtonMaterialSetCollisionCallback(m_world, firstMaterial, secondMaterial, callbackPtr.get(), nullptr, ProcessContact);

		Nz::UInt64 firstMaterialId(firstMaterial);
		Nz::UInt64 secondMaterialId(secondMaterial);

		Nz::UInt64 callbackIndex = firstMaterialId << 32 | secondMaterialId;
		m_callbacks[callbackIndex] = std::move(callbackPtr);
	}

	void PhysWorld3D::Step(float timestep)
	{
		m_timestepAccumulator += timestep;

		while (m_timestepAccumulator >= m_stepSize)
		{
			NewtonUpdate(m_world, m_stepSize);
			m_timestepAccumulator -= m_stepSize;
		}
	}

	void PhysWorld3D::ProcessContact(const NewtonJoint* const contactJoint, float timestep, int threadIndex)
	{
		Nz::RigidBody3D* bodyA = static_cast<Nz::RigidBody3D*>(NewtonBodyGetUserData(NewtonJointGetBody0(contactJoint)));
		Nz::RigidBody3D* bodyB = static_cast<Nz::RigidBody3D*>(NewtonBodyGetUserData(NewtonJointGetBody1(contactJoint)));
		assert(bodyA && bodyB);

		using ContactJoint = void*;

		// Query all joints first, to prevent removing a joint from the list while iterating on it
		Nz::StackArray<ContactJoint> contacts = NazaraStackAllocationNoInit(ContactJoint, NewtonContactJointGetContactCount(contactJoint));
		std::size_t contactIndex = 0;
		for (ContactJoint contact = NewtonContactJointGetFirstContact(contactJoint); contact; contact = NewtonContactJointGetNextContact(contactJoint, contact))
		{
			assert(contactIndex < contacts.size());
			contacts[contactIndex++] = contact;
		}

		for (ContactJoint contact : contacts)
		{
			NewtonMaterial* material = NewtonContactGetMaterial(contact);
			Callback* callbackData = static_cast<Callback*>(NewtonMaterialGetMaterialPairUserData(material));
			assert(callbackData);

			if (!callbackData->collisionCallback(*bodyA, *bodyB))
				NewtonContactJointRemoveContact(contactJoint, contact);
		}
	}
}
