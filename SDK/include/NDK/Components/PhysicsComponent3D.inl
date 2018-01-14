// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>
#include "PhysicsComponent3D.hpp"

namespace Ndk
{
	inline PhysicsComponent3D::PhysicsComponent3D() :
	m_nodeSynchronizationEnabled(true)
	{
	}

	/*!
	* \brief Constructs a PhysicsComponent3D object by copy semantic
	*
	* \param physics PhysicsComponent3D to copy
	*/
	inline PhysicsComponent3D::PhysicsComponent3D(const PhysicsComponent3D& physics) :
	m_nodeSynchronizationEnabled(physics.m_nodeSynchronizationEnabled)
	{
		// We can't make a copy of the RigidBody3D, as we are not attached yet (and will possibly be attached to another world)
		CopyPhysicsState(physics.GetRigidBody());
	}

	/*!
	* \brief Applies a force to the entity
	*
	* \param force Force to apply on the entity
	* \param coordSys System coordinates to consider
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline void PhysicsComponent3D::AddForce(const Nz::Vector3f& force, Nz::CoordSys coordSys)
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

	inline void PhysicsComponent3D::AddForce(const Nz::Vector3f& force, const Nz::Vector3f& point, Nz::CoordSys coordSys)
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

	inline void PhysicsComponent3D::AddTorque(const Nz::Vector3f& torque, Nz::CoordSys coordSys)
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
	inline void PhysicsComponent3D::EnableAutoSleep(bool autoSleep)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->EnableAutoSleep(autoSleep);
	}

	/*!
	* \brief Enables position/rotation synchronization with the NodeComponent
	*
	* By default, at every update of the PhysicsSystem3D, the NodeComponent's position and rotation (if any) will be synchronized with
	* the values of the PhysicsComponent3D. This function allows to enable/disable this behavior on a per-entity basis.
	*
	* \param nodeSynchronization Should synchronization occur between NodeComponent and PhysicsComponent3D
	*/
	inline void PhysicsComponent3D::EnableNodeSynchronization(bool nodeSynchronization)
	{
		m_nodeSynchronizationEnabled = nodeSynchronization;

		if (m_entity)
			m_entity->Invalidate();
	}

	/*!
	* \brief Gets the AABB of the physics object
	* \return AABB of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline Nz::Boxf PhysicsComponent3D::GetAABB() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetAABB();
	}

	/*!
	* \brief Gets the angular damping of the physics object
	* \return Angular damping of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline Nz::Vector3f PhysicsComponent3D::GetAngularDamping() const
	{
		return m_object->GetAngularDamping();
	}

	/*!
	* \brief Gets the angular velocity of the physics object
	* \return Angular velocity of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline Nz::Vector3f PhysicsComponent3D::GetAngularVelocity() const
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
	inline float PhysicsComponent3D::GetGravityFactor() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetGravityFactor();
	}

	/*!
	* \brief Gets the linear damping of the physics object
	* \return Linear damping of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline float PhysicsComponent3D::GetLinearDamping() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetLinearDamping();
	}

	/*!
	* \brief Gets the linear velocity of the physics object
	* \return Linear velocity of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline Nz::Vector3f PhysicsComponent3D::GetLinearVelocity() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetLinearVelocity();
	}

	/*!
	* \brief Gets the mass of the physics object
	* \return Mass of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline float PhysicsComponent3D::GetMass() const
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

	inline Nz::Vector3f PhysicsComponent3D::GetMassCenter(Nz::CoordSys coordSys) const
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

	inline const Nz::Matrix4f& PhysicsComponent3D::GetMatrix() const
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

	inline Nz::Vector3f PhysicsComponent3D::GetPosition() const
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

	inline Nz::Quaternionf PhysicsComponent3D::GetRotation() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetRotation();
	}

	/*!
	* \brief Checks whether the auto sleep is enabled
	* \return true If it is the case
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline bool PhysicsComponent3D::IsAutoSleepEnabled() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->IsAutoSleepEnabled();
	}

	/*!
	* \brief Checks whether the object is moveable
	* \return true If it is the case
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline bool PhysicsComponent3D::IsMoveable() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->IsMoveable();
	}

	/*!
	* \brief Checks if position & rotation are synchronized with NodeComponent
	* \return true If synchronization is enabled
	*
	* \see EnableNodeSynchronization
	*/
	inline bool PhysicsComponent3D::IsNodeSynchronizationEnabled() const
	{
		return m_nodeSynchronizationEnabled;
	}

	/*!
	* \brief Checks whether the entity is currently sleeping
	* \return true If it is the case
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline bool PhysicsComponent3D::IsSleeping() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->IsSleeping();
	}

	/*!
	* \brief Sets the angular damping of the physics object
	*
	* \param angularDamping Angular damping of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline void PhysicsComponent3D::SetAngularDamping(const Nz::Vector3f& angularDamping)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetAngularDamping(angularDamping);
	}

	/*!
	* \brief Sets the angular velocity of the physics object
	*
	* \param angularVelocity Angular velocity of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline void PhysicsComponent3D::SetAngularVelocity(const Nz::Vector3f& angularVelocity)
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
	inline void PhysicsComponent3D::SetGravityFactor(float gravityFactor)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetGravityFactor(gravityFactor);
	}

	/*!
	* \brief Sets the linear damping of the physics object
	*
	* \param damping Linear damping of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline void PhysicsComponent3D::SetLinearDamping(float damping)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetLinearDamping(damping);
	}

	/*!
	* \brief Sets the linear velocity of the physics object
	*
	* \param velocity New linear velocity of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline void PhysicsComponent3D::SetLinearVelocity(const Nz::Vector3f& velocity)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetLinearVelocity(velocity);
	}

	/*!
	* \brief Sets the mass of the physics object
	*
	* \param mass Mass of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	* \remark Produces a NazaraAssert if the mass is negative
	*/
	inline void PhysicsComponent3D::SetMass(float mass)
	{
		NazaraAssert(m_object, "Invalid physics object");
		NazaraAssert(mass >= 0.f, "Mass must be positive and finite");
		NazaraAssert(std::isfinite(mass), "Mass must be positive and finite");

		m_object->SetMass(mass);
	}

	/*!
	* \brief Sets the gravity center of the physics object
	*
	* \param center Gravity center of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline void PhysicsComponent3D::SetMassCenter(const Nz::Vector3f& center)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetMassCenter(center);
	}

	/*!
	* \brief Sets the material of the object, affecting how object does respond to collisions
	*
	* \param materialName Name of the material, previously registered to physics world
	*
	* \remark materialName must exists in PhysWorld before this call
	*/
	inline void PhysicsComponent3D::SetMaterial(const Nz::String& materialName)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetMaterial(materialName);
	}

	/*!
	* \brief Sets the material of the object, affecting how object does respond to collisions
	*
	* \param materialIndex Id of the material, previously retrieved from a physics world
	*
	* \remark materialIndex must come from a call to in PhysWorld::CreateMaterial
	*/
	inline void PhysicsComponent3D::SetMaterial(int materialIndex)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetMaterial(materialIndex);
	}

	/*!
	* \brief Sets the position of the physics object
	*
	* \param position Position of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline void PhysicsComponent3D::SetPosition(const Nz::Vector3f& position)
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

	inline void PhysicsComponent3D::SetRotation(const Nz::Quaternionf& rotation)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetRotation(rotation);
	}

	inline void PhysicsComponent3D::ApplyPhysicsState(Nz::RigidBody3D& rigidBody) const
	{
		assert(m_pendingStates.valid);

		rigidBody.EnableAutoSleep(m_pendingStates.autoSleep);
		rigidBody.SetAngularDamping(m_pendingStates.angularDamping);
		rigidBody.SetGravityFactor(m_pendingStates.gravityFactor);
		rigidBody.SetLinearDamping(m_pendingStates.linearDamping);
		rigidBody.SetMass(m_pendingStates.mass);
		rigidBody.SetMassCenter(m_pendingStates.massCenter);
	}

	inline void PhysicsComponent3D::CopyPhysicsState(const Nz::RigidBody3D& rigidBody)
	{
		m_pendingStates.autoSleep = rigidBody.IsAutoSleepEnabled();
		m_pendingStates.angularDamping = rigidBody.GetAngularDamping();
		m_pendingStates.gravityFactor = rigidBody.GetGravityFactor();
		m_pendingStates.linearDamping = rigidBody.GetLinearDamping();
		m_pendingStates.mass = rigidBody.GetMass();
		m_pendingStates.massCenter = rigidBody.GetMassCenter(Nz::CoordSys_Local);
		m_pendingStates.valid = true;
	}

	/*!
	* \brief Gets the underlying physics object
	* \return A reference to the physics object
	*/
	inline Nz::RigidBody3D* PhysicsComponent3D::GetRigidBody()
	{
		return m_object.get();
	}

	/*!
	* \brief Gets the underlying physics object
	* \return A reference to the physics object
	*/
	inline const Nz::RigidBody3D& PhysicsComponent3D::GetRigidBody() const
	{
		return *m_object.get();
	}
}
