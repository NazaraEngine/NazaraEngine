// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>

namespace Ndk
{
	inline PhysicsComponent::PhysicsComponent(const PhysicsComponent& physics)
	{
		// Pas de copie de l'objet physique (étant donné que nous n'allons le créer qu'une fois attaché à une entité)
		NazaraUnused(physics);
	}

	inline void PhysicsComponent::AddForce(const NzVector3f& force, nzCoordSys coordSys)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->AddForce(force, coordSys);
	}

	inline void PhysicsComponent::AddForce(const NzVector3f& force, const NzVector3f& point, nzCoordSys coordSys)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->AddForce(force, point, coordSys);
	}

	inline void PhysicsComponent::AddTorque(const NzVector3f& torque, nzCoordSys coordSys)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->AddForce(torque, coordSys);
	}

	inline void PhysicsComponent::EnableAutoSleep(bool autoSleep)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->EnableAutoSleep(autoSleep);
	}

	inline NzBoxf PhysicsComponent::GetAABB() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetAABB();
	}

	inline NzVector3f PhysicsComponent::GetAngularVelocity() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetAngularVelocity();
	}

	inline float PhysicsComponent::GetGravityFactor() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetGravityFactor();
	}

	inline float PhysicsComponent::GetMass() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetMass();
	}

	inline NzVector3f PhysicsComponent::GetMassCenter(nzCoordSys coordSys) const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetMassCenter(coordSys);
	}

	inline const NzMatrix4f& PhysicsComponent::GetMatrix() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetMatrix();
	}

	inline NzVector3f PhysicsComponent::GetPosition() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetPosition();
	}

	inline NzQuaternionf PhysicsComponent::GetRotation() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetRotation();
	}

	inline NzVector3f PhysicsComponent::GetVelocity() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetVelocity();
	}

	inline bool PhysicsComponent::IsAutoSleepEnabled() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->IsAutoSleepEnabled();
	}

	inline bool PhysicsComponent::IsSleeping() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->IsSleeping();
	}

	inline void PhysicsComponent::SetAngularVelocity(const NzVector3f& angularVelocity)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetAngularVelocity(angularVelocity);
	}

	inline void PhysicsComponent::SetGravityFactor(float gravityFactor)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetGravityFactor(gravityFactor);
	}

	inline void PhysicsComponent::SetMass(float mass)
	{
		NazaraAssert(m_object, "Invalid physics object");
		NazaraAssert(mass > 0.f, "Mass should be positive");

		m_object->SetMass(mass);
	}

	inline void PhysicsComponent::SetMassCenter(const NzVector3f& center)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetMassCenter(center);
	}

	inline void PhysicsComponent::SetPosition(const NzVector3f& position)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetPosition(position);
	}

	inline void PhysicsComponent::SetRotation(const NzQuaternionf& rotation)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetRotation(rotation);
	}

	inline void PhysicsComponent::SetVelocity(const NzVector3f& velocity)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetVelocity(velocity);
	}

	inline NzPhysObject& PhysicsComponent::GetPhysObject()
	{
		return *m_object.get();
	}
}
