// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>

namespace Ndk
{
	/*!
	* \brief Constructs a PhysicsComponent object by copy semantic
	*
	* \param physics PhysicsComponent to copy
	*/

	inline PhysicsComponent::PhysicsComponent(const PhysicsComponent& physics)
	{
		// No copy of physical object (because we only create it when attached to an entity)
		NazaraUnused(physics);
	}

	/*!
	* \brief Applies a force to the entity
	*
	* \param force Force to apply on the entity
	* \param coordSys System coordinates to consider
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline void PhysicsComponent::AddForce(const Nz::Vector3f& force, Nz::CoordSys coordSys)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->AddForce(force, coordSys);
	}

	/*!
	* \brief Applies a force to the entity
	*
	* \param force Force to apply on the entity
	* \param point Point where to apply the force
	* \param coordSys System coordinates to consider
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline void PhysicsComponent::AddForce(const Nz::Vector3f& force, const Nz::Vector3f& point, Nz::CoordSys coordSys)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->AddForce(force, point, coordSys);
	}

	/*!
	* \brief Applies a torque to the entity
	*
	* \param torque Torque to apply on the entity
	* \param coordSys System coordinates to consider
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline void PhysicsComponent::AddTorque(const Nz::Vector3f& torque, Nz::CoordSys coordSys)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->AddTorque(torque, coordSys);
	}

	/*!
	* \brief Enables auto sleep of physics object
	*
	* \param autoSleep Should the physics of the object be disabled when too far from others
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline void PhysicsComponent::EnableAutoSleep(bool autoSleep)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->EnableAutoSleep(autoSleep);
	}

	/*!
	* \brief Gets the AABB of the physics object
	* \return AABB of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline Nz::Boxf PhysicsComponent::GetAABB() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetAABB();
	}

	/*!
	* \brief Gets the angular velocity of the physics object
	* \return Angular velocity of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline Nz::Vector3f PhysicsComponent::GetAngularVelocity() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetAngularVelocity();
	}

	/*!
	* \brief Gets the gravity factor of the physics object
	* \return Gravity factor of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline float PhysicsComponent::GetGravityFactor() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetGravityFactor();
	}

	/*!
	* \brief Gets the mass of the physics object
	* \return Mass of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline float PhysicsComponent::GetMass() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetMass();
	}

	/*!
	* \brief Gets the gravity center of the physics object
	* \return Gravity center of the object
	*
	* \param coordSys System coordinates to consider 
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline Nz::Vector3f PhysicsComponent::GetMassCenter(Nz::CoordSys coordSys) const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetMassCenter(coordSys);
	}

	/*!
	* \brief Gets the matrix of the physics object
	* \return Matrix of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline const Nz::Matrix4f& PhysicsComponent::GetMatrix() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetMatrix();
	}

	/*!
	* \brief Gets the position of the physics object
	* \return Position of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline Nz::Vector3f PhysicsComponent::GetPosition() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetPosition();
	}

	/*!
	* \brief Gets the rotation of the physics object
	* \return Rotation of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline Nz::Quaternionf PhysicsComponent::GetRotation() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetRotation();
	}

	/*!
	* \brief Gets the velocity of the physics object
	* \return Velocity of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline Nz::Vector3f PhysicsComponent::GetVelocity() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetVelocity();
	}

	/*!
	* \brief Checks whether the auto sleep is enabled
	* \return true If it is the case
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline bool PhysicsComponent::IsAutoSleepEnabled() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->IsAutoSleepEnabled();
	}

	/*!
	* \brief Checks whether the entity is currently sleeping
	* \return true If it is the case
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline bool PhysicsComponent::IsSleeping() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->IsSleeping();
	}

	/*!
	* \brief Sets the angular velocity of the physics object
	*
	* \param angularVelocity Angular velocity of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline void PhysicsComponent::SetAngularVelocity(const Nz::Vector3f& angularVelocity)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetAngularVelocity(angularVelocity);
	}

	/*!
	* \brief Sets the gravity factor of the physics object
	*
	* \param gravityFactor Gravity factor of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline void PhysicsComponent::SetGravityFactor(float gravityFactor)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetGravityFactor(gravityFactor);
	}

	/*!
	* \brief Sets the mass of the physics object
	*
	* \param mass Mass of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	* \remark Produces a NazaraAssert if the mass is negative
	*/

	inline void PhysicsComponent::SetMass(float mass)
	{
		NazaraAssert(m_object, "Invalid physics object");
		NazaraAssert(mass > 0.f, "Mass should be positive");

		m_object->SetMass(mass);
	}

	/*!
	* \brief Sets the gravity center of the physics object
	*
	* \param center Gravity center of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline void PhysicsComponent::SetMassCenter(const Nz::Vector3f& center)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetMassCenter(center);
	}

	/*!
	* \brief Sets the position of the physics object
	*
	* \param position Position of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline void PhysicsComponent::SetPosition(const Nz::Vector3f& position)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetPosition(position);
	}

	/*!
	* \brief Sets the rotation of the physics object
	*
	* \param rotation Rotation of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline void PhysicsComponent::SetRotation(const Nz::Quaternionf& rotation)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetRotation(rotation);
	}

	/*!
	* \brief Sets the velocity of the physics object
	*
	* \param velocity Velocity of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline void PhysicsComponent::SetVelocity(const Nz::Vector3f& velocity)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetVelocity(velocity);
	}

	/*!
	* \brief Gets the underlying physics object
	* \return A reference to the physics object
	*/

	inline Nz::PhysObject& PhysicsComponent::GetPhysObject()
	{
		return *m_object.get();
	}
}
