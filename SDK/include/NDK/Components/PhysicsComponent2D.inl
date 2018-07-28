// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Components/PhysicsComponent2D.hpp>
#include <Nazara/Core/Error.hpp>

namespace Ndk
{
	/*!
	* \brief Constructs a PhysicsComponent2D object by copy semantic
	*
	* \param physics PhysicsComponent2D to copy
	*/

	inline PhysicsComponent2D::PhysicsComponent2D(const PhysicsComponent2D& physics)
	{
		// No copy of physical object (because we only create it when attached to an entity)
		NazaraUnused(physics);
	}

	/*!
	* \brief Applies a physics force to the entity
	*
	* \param force Force to apply on the entity
	* \param coordSys System coordinates to consider
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline void PhysicsComponent2D::AddForce(const Nz::Vector2f& force, Nz::CoordSys coordSys)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->AddForce(force, coordSys);
	}

	/*!
	* \brief Applies a physics force to the entity
	*
	* \param force Force to apply on the entity
	* \param point Point where to apply the force
	* \param coordSys System coordinates to consider
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline void PhysicsComponent2D::AddForce(const Nz::Vector2f& force, const Nz::Vector2f& point, Nz::CoordSys coordSys)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->AddForce(force, point, coordSys);
	}
	
	/*!
	* \brief Applies a impulse to the entity
	*
	* \param impulse Impulse to apply on the entity
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	
	inline void PhysicsComponent2D::AddImpulse(const Nz::Vector2f& impulse, Nz::CoordSys coordSys)
	{
		NazaraAssert(m_object, "Invalid physics object");
		
		m_object->AddImpulse(impulse, coordSys);
	}
	
	/*!
	* \brief Applies a impulse to the entity
	*
	* \param impulse Impulse to apply on the entity
	* \param point Point where the impulse is applied
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	
	inline void PhysicsComponent2D::AddImpulse(const Nz::Vector2f& impulse, const Nz::Vector2f& point, Nz::CoordSys coordSys)
	{
		NazaraAssert(m_object, "Invalid physics object");
		
		m_object->AddImpulse(impulse, point, coordSys);
	}
	
	/*!
	* \brief Applies a torque to the entity
	*
	* \param torque Torque to apply on the entity
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline void PhysicsComponent2D::AddTorque(float torque)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->AddTorque(torque);
	}

	/*!
	* \brief Finds the closest point on the entity relative to a position
	* \return True if such a point exists (will return false if no collider exists)
	*
	* \param position The starting point which will be used for the query
	* \param closestPoint The closest point on entity surface
	* \param closestDistance The distance between the closest point and the starting point, may be negative if starting point is inside the entity
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline bool PhysicsComponent2D::ClosestPointQuery(const Nz::Vector2f& position, Nz::Vector2f* closestPoint, float* closestDistance) const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->ClosestPointQuery(position, closestPoint, closestDistance);
	}

	/*!
	* \brief Gets the AABB of the physics object
	* \return AABB of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline Nz::Rectf PhysicsComponent2D::GetAABB() const
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

	inline float PhysicsComponent2D::GetAngularVelocity() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetAngularVelocity();
	}

	/*!
	* \brief Gets the gravity center of the physics object
	* \return Gravity center of the object
	*
	* \param coordSys System coordinates to consider
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline Nz::Vector2f PhysicsComponent2D::GetCenterOfGravity(Nz::CoordSys coordSys) const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetCenterOfGravity(coordSys);
	}

	/*!
	* \brief Gets the mass of the physics object
	* \return Mass of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline float PhysicsComponent2D::GetMass() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetMass();
	}

	/*!
	* \brief Gets the position of the physics object
	* \return Position of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline Nz::Vector2f PhysicsComponent2D::GetPosition() const
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

	inline float PhysicsComponent2D::GetRotation() const
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

	inline Nz::Vector2f PhysicsComponent2D::GetVelocity() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetVelocity();
	}

	/*!
	* \brief Checks whether the entity is currently sleeping
	* \return true If it is the case
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline bool PhysicsComponent2D::IsSleeping() const
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

	inline void PhysicsComponent2D::SetAngularVelocity(float angularVelocity)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetAngularVelocity(angularVelocity);
	}

	/*!
	* \brief Sets the mass of the physics object
	*
	* \param mass Mass of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	* \remark Produces a NazaraAssert if the mass is negative
	*/

	inline void PhysicsComponent2D::SetMass(float mass)
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

	inline void PhysicsComponent2D::SetMassCenter(const Nz::Vector2f& center)
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

	inline void PhysicsComponent2D::SetPosition(const Nz::Vector2f& position)
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

	inline void PhysicsComponent2D::SetRotation(float rotation)
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

	inline void PhysicsComponent2D::SetVelocity(const Nz::Vector2f& velocity)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetVelocity(velocity);
	}

	/*!
	* \brief Gets the underlying physics object
	* \return A reference to the physics object
	*/

	inline Nz::RigidBody2D* PhysicsComponent2D::GetRigidBody()
	{
		return m_object.get();
	}
}
