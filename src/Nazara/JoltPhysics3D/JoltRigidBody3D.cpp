// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/JoltRigidBody3D.hpp>
#include <Nazara/JoltPhysics3D/JoltHelper.hpp>
#include <Nazara/JoltPhysics3D/JoltPhysWorld3D.hpp>
#include <Nazara/JoltPhysics3D/JoltRigidBody3D.hpp>
#include <Nazara/Utils/MemoryHelper.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	JoltRigidBody3D::JoltRigidBody3D(JoltPhysWorld3D* world, const Matrix4f& mat) :
	JoltRigidBody3D(world, nullptr, mat)
	{
	}

	JoltRigidBody3D::JoltRigidBody3D(JoltPhysWorld3D* world, std::shared_ptr<JoltCollider3D> geom, const Matrix4f& mat) :
	m_geom(std::move(geom)),
	m_world(world)
	{
		NazaraAssert(m_world, "Invalid world");

		if (!m_geom)
			m_geom = std::make_shared<JoltSphereCollider3D>(1.f);

		JPH::BodyInterface& body_interface = m_world->GetPhysicsSystem()->GetBodyInterface();

		JPH::Vec3 position = ToJolt(mat.GetTranslation());
		JPH::Quat rotation = ToJolt(mat.GetRotation().Normalize());

		JPH::BodyCreationSettings settings(m_geom->GetShapeSettings(), position, rotation, JPH::EMotionType::Dynamic, 1);

		JPH::Body* body = body_interface.CreateBody(settings);
		body->SetUserData(SafeCast<UInt64>(reinterpret_cast<std::uintptr_t>(this)));

		body_interface.AddBody(body->GetID(), JPH::EActivation::Activate);
		m_bodyIndex = body->GetID().GetIndexAndSequenceNumber();
	}

	JoltRigidBody3D::JoltRigidBody3D(JoltRigidBody3D&& object) noexcept :
	m_geom(std::move(object.m_geom)),
	m_bodyIndex(object.m_bodyIndex),
	m_world(object.m_world)
	{
		object.m_bodyIndex = JPH::BodyID::cInvalidBodyID;
	}

	JoltRigidBody3D::~JoltRigidBody3D()
	{
		Destroy();
	}

	void JoltRigidBody3D::AddForce(const Vector3f& force, CoordSys coordSys)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
			{
				JPH::BodyInterface& body_interface = m_world->GetPhysicsSystem()->GetBodyInterface();
				body_interface.AddForce(JPH::BodyID(m_bodyIndex), ToJolt(force));
				//WakeUp();
				//m_body->applyCentralForce(ToJolt(force));
				break;
			}

			case CoordSys::Local:
				//WakeUp();
				//m_body->applyCentralForce(ToJolt(GetRotation() * force));
				break;
		}
	}

#if 0
	void JoltRigidBody3D::AddForce(const Vector3f& force, const Vector3f& point, CoordSys coordSys)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
				WakeUp();
				m_body->applyForce(ToJolt(force), ToJolt(point));
				break;

			case CoordSys::Local:
			{
				Matrix4f transformMatrix = GetMatrix();
				return AddForce(transformMatrix.Transform(force, 0.f), point, CoordSys::Global);
			}
		}
	}

	void JoltRigidBody3D::AddTorque(const Vector3f& torque, CoordSys coordSys)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
				WakeUp();
				m_body->applyTorque(ToJolt(torque));
				break;

			case CoordSys::Local:
				Matrix4f transformMatrix = GetMatrix();
				WakeUp();
				m_body->applyTorque(ToJolt(transformMatrix.Transform(torque, 0.f)));
				break;
		}
	}
#endif
	void JoltRigidBody3D::EnableSleeping(bool enable)
	{
		auto& body_interface = m_world->GetPhysicsSystem()->GetBodyLockInterface();
		JPH::BodyLockWrite lock(body_interface, JPH::BodyID(m_bodyIndex));
		if (lock.Succeeded())
		{
			JPH::Body& body = lock.GetBody();
			body.SetAllowSleeping(enable);
		}
	}

#if 0
	void JoltRigidBody3D::FallAsleep()
	{
		if (m_body->getActivationState() != DISABLE_DEACTIVATION)
			m_body->setActivationState(ISLAND_SLEEPING);
	}

	Boxf JoltRigidBody3D::GetAABB() const
	{
		btVector3 min, max;
		m_body->getAabb(min, max);

		return Boxf(FromJolt(min), FromJolt(max));
	}

	float JoltRigidBody3D::GetAngularDamping() const
	{
		return m_body->getAngularDamping();
	}

	Vector3f JoltRigidBody3D::GetAngularVelocity() const
	{
		return FromJolt(m_body->getAngularVelocity());
	}

	float JoltRigidBody3D::GetLinearDamping() const
	{
		return m_body->getLinearDamping();
	}

	Vector3f JoltRigidBody3D::GetLinearVelocity() const
	{
		return FromJolt(m_body->getLinearVelocity());
	}
#endif

	float JoltRigidBody3D::GetMass() const
	{
		auto& body_interface = m_world->GetPhysicsSystem()->GetBodyLockInterface();
		JPH::BodyLockRead lock(body_interface, JPH::BodyID(m_bodyIndex));
		if (!lock.Succeeded())
			return 0.f;

		const JPH::Body& body = lock.GetBody();
		return 1.f / body.GetMotionProperties()->GetInverseMass();
	}
#if 0
	Vector3f JoltRigidBody3D::GetMassCenter(CoordSys coordSys) const
	{
		return FromJolt(m_body->getCenterOfMassPosition());
	}

	Matrix4f JoltRigidBody3D::GetMatrix() const
	{
		return FromJolt(m_body->getWorldTransform());
	}
#endif

	Vector3f JoltRigidBody3D::GetPosition() const
	{
		JPH::BodyInterface& body_interface = m_world->GetPhysicsSystem()->GetBodyInterface();
		return FromJolt(body_interface.GetPosition(JPH::BodyID(m_bodyIndex)));
	}

	Quaternionf JoltRigidBody3D::GetRotation() const
	{
		JPH::BodyInterface& body_interface = m_world->GetPhysicsSystem()->GetBodyInterface();
		return FromJolt(body_interface.GetRotation(JPH::BodyID(m_bodyIndex)));
	}

#if 0
	bool JoltRigidBody3D::IsSimulationEnabled() const
	{
		return m_body->isActive();
	}
#endif

	bool JoltRigidBody3D::IsSleeping() const
	{
		JPH::BodyInterface& body_interface = m_world->GetPhysicsSystem()->GetBodyInterface();
		return !body_interface.IsActive(JPH::BodyID(m_bodyIndex));
	}

	bool JoltRigidBody3D::IsSleepingEnabled() const
	{
		auto& body_interface = m_world->GetPhysicsSystem()->GetBodyLockInterface();
		JPH::BodyLockRead lock(body_interface, JPH::BodyID(m_bodyIndex));
		if (!lock.Succeeded())
			return true;

		const JPH::Body& body = lock.GetBody();
		return body.GetAllowSleeping();
	}

#if 0
	void JoltRigidBody3D::SetAngularDamping(float angularDamping)
	{
		m_body->setDamping(m_body->getLinearDamping(), angularDamping);
	}

	void JoltRigidBody3D::SetAngularVelocity(const Vector3f& angularVelocity)
	{
		m_body->setAngularVelocity(ToJolt(angularVelocity));
	}
#endif
	void JoltRigidBody3D::SetGeom(std::shared_ptr<JoltCollider3D> geom, bool recomputeInertia)
	{
		if (m_geom != geom)
		{
			if (geom)
				m_geom = std::move(geom);
			else
				m_geom = std::make_shared<JoltSphereCollider3D>(1.f);

			JPH::BodyInterface& body_interface = m_world->GetPhysicsSystem()->GetBodyInterface();
			body_interface.SetShape(JPH::BodyID(m_bodyIndex), m_geom->GetShapeSettings()->Create().Get(), true, JPH::EActivation::Activate);
		}
	}
#if 0
	void JoltRigidBody3D::SetLinearDamping(float damping)
	{
		m_body->setDamping(damping, m_body->getAngularDamping());
	}

	void JoltRigidBody3D::SetLinearVelocity(const Vector3f& velocity)
	{
		m_body->setLinearVelocity(ToJolt(velocity));
	}
#endif 0
	void JoltRigidBody3D::SetMass(float mass)
	{
		NazaraAssert(mass >= 0.f, "Mass must be positive and finite");
		NazaraAssert(std::isfinite(mass), "Mass must be positive and finite");

		auto& bodyLock = m_world->GetPhysicsSystem()->GetBodyLockInterface();
		JPH::BodyLockWrite lock(bodyLock, JPH::BodyID(m_bodyIndex));
		if (!lock.Succeeded())
			return;

		JPH::Body& body = lock.GetBody();

		if (mass > 0.f)
		{
			body.SetMotionType(JPH::EMotionType::Dynamic);
			body.GetMotionProperties()->SetInverseMass(1.f / mass);
		}
		else
		{
			JPH::BodyInterface& body_interface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();

			body_interface.DeactivateBody(body.GetID());
			body.SetMotionType(JPH::EMotionType::Static);
		}
	}

#if 0
	void JoltRigidBody3D::SetMassCenter(const Vector3f& center)
	{
		btTransform centerTransform;
		centerTransform.setIdentity();
		centerTransform.setOrigin(ToJolt(center));

		m_body->setCenterOfMassTransform(centerTransform);
	}
#endif 0
	void JoltRigidBody3D::SetPosition(const Vector3f& position)
	{
		JPH::BodyInterface& body_interface = m_world->GetPhysicsSystem()->GetBodyInterface();
		body_interface.SetPosition(JPH::BodyID(m_bodyIndex), ToJolt(position), JPH::EActivation::Activate);
	}

	void JoltRigidBody3D::SetRotation(const Quaternionf& rotation)
	{
		JPH::BodyInterface& body_interface = m_world->GetPhysicsSystem()->GetBodyInterface();
		body_interface.SetRotation(JPH::BodyID(m_bodyIndex), ToJolt(rotation), JPH::EActivation::Activate);
	}

#if 0
	Quaternionf JoltRigidBody3D::ToLocal(const Quaternionf& worldRotation)
	{
		return GetRotation().Conjugate() * worldRotation;
	}

	Vector3f JoltRigidBody3D::ToLocal(const Vector3f& worldPosition)
	{
		btTransform worldTransform = m_body->getWorldTransform();
		return GetMatrix().InverseTransform() * worldPosition;
	}

	Quaternionf JoltRigidBody3D::ToWorld(const Quaternionf& localRotation)
	{
		return GetRotation() * localRotation;
	}

	Vector3f JoltRigidBody3D::ToWorld(const Vector3f& localPosition)
	{
		return GetMatrix() * localPosition;
	}
#endif

	void JoltRigidBody3D::WakeUp()
	{
		JPH::BodyInterface& body_interface = m_world->GetPhysicsSystem()->GetBodyInterface();
		body_interface.ActivateBody(JPH::BodyID(m_bodyIndex)); 
	}

	JoltRigidBody3D& JoltRigidBody3D::operator=(JoltRigidBody3D&& object) noexcept
	{
		Destroy();

		m_bodyIndex     = object.m_bodyIndex;
		m_geom          = std::move(object.m_geom);
		m_world         = object.m_world;

		object.m_bodyIndex = JPH::BodyID::cInvalidBodyID;

		return *this;
	}

	void JoltRigidBody3D::Destroy()
	{
		if (m_bodyIndex != JPH::BodyID::cInvalidBodyID)
		{
			JPH::BodyInterface& body_interface = m_world->GetPhysicsSystem()->GetBodyInterface();
			body_interface.RemoveBody(JPH::BodyID(m_bodyIndex));
			body_interface.DestroyBody(JPH::BodyID(m_bodyIndex));
			m_bodyIndex = JPH::BodyID::cInvalidBodyID;
		}

		m_geom.reset();
	}
}
