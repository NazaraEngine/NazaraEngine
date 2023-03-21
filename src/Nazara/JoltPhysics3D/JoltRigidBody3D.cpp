// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/JoltRigidBody3D.hpp>
#include <Nazara/JoltPhysics3D/JoltHelper.hpp>
#include <Nazara/JoltPhysics3D/JoltPhysWorld3D.hpp>
#include <Nazara/JoltPhysics3D/JoltRigidBody3D.hpp>
#include <NazaraUtils/MemoryHelper.hpp>
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
			m_geom = std::make_shared<JoltSphereCollider3D>(std::numeric_limits<float>::epsilon());

		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterface();

		JPH::Vec3 position = ToJolt(mat.GetTranslation());
		JPH::Quat rotation = ToJolt(mat.GetRotation().Normalize());

		JPH::BodyCreationSettings settings(m_geom->GetShapeSettings(), position, rotation, JPH::EMotionType::Dynamic, 1);

		m_body = bodyInterface.CreateBody(settings);
		m_body->SetUserData(SafeCast<UInt64>(reinterpret_cast<std::uintptr_t>(this)));

		JPH::BodyID bodyId = m_body->GetID();

		bodyInterface.AddBody(bodyId, JPH::EActivation::Activate);
		m_bodyIndex = bodyId.GetIndex();
	}

	JoltRigidBody3D::JoltRigidBody3D(JoltRigidBody3D&& object) noexcept :
	m_geom(std::move(object.m_geom)),
	m_body(object.m_body),
	m_bodyIndex(object.m_bodyIndex),
	m_world(object.m_world)
	{
		object.m_body = nullptr;
		object.m_bodyIndex = std::numeric_limits<UInt32>::max();
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
				m_body->AddForce(ToJolt(force));
				break;
			}

			case CoordSys::Local:
				m_body->AddForce(m_body->GetRotation() * ToJolt(force));
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
		m_body->SetAllowSleeping(enable);
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
		return 1.f / m_body->GetMotionProperties()->GetInverseMass();
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
		return FromJolt(m_body->GetPosition());
	}

	std::pair<Vector3f, Quaternionf> JoltRigidBody3D::GetPositionAndRotation() const
	{
		JPH::Vec3 position = m_body->GetPosition();
		JPH::Quat rotation = m_body->GetRotation();

		return { FromJolt(position), FromJolt(rotation) };
	}

	Quaternionf JoltRigidBody3D::GetRotation() const
	{
		return FromJolt(m_body->GetRotation());
	}

#if 0
	bool JoltRigidBody3D::IsSimulationEnabled() const
	{
		return m_body->isActive();
	}
#endif

	bool JoltRigidBody3D::IsSleeping() const
	{
		return m_body->IsActive();
	}

	bool JoltRigidBody3D::IsSleepingEnabled() const
	{
		return m_body->GetAllowSleeping();
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
			float mass = GetMass();

			if (geom)
				m_geom = std::move(geom);
			else
				m_geom = std::make_shared<JoltSphereCollider3D>(std::numeric_limits<float>::epsilon());

			JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterface();
			bodyInterface.SetShape(m_body->GetID(), m_geom->GetShapeSettings()->Create().Get(), false, JPH::EActivation::Activate);
			if (recomputeInertia)
			{
				JPH::MassProperties massProperties = m_body->GetShape()->GetMassProperties();
				massProperties.ScaleToMass(mass);
				m_body->GetMotionProperties()->SetMassProperties(massProperties);
			}
		}
	}

	void JoltRigidBody3D::SetLinearDamping(float damping)
	{
		m_body->GetMotionProperties()->SetLinearDamping(damping);
	}

	void JoltRigidBody3D::SetLinearVelocity(const Vector3f& velocity)
	{
		m_body->SetLinearVelocity(ToJolt(velocity));
	}

	void JoltRigidBody3D::SetMass(float mass, bool recomputeInertia)
	{
		NazaraAssert(mass >= 0.f, "Mass must be positive and finite");
		NazaraAssert(std::isfinite(mass), "Mass must be positive and finite");

		if (mass > 0.f)
		{
			m_body->SetMotionType(JPH::EMotionType::Dynamic);
			if (recomputeInertia)
			{
				JPH::MassProperties massProperties = m_body->GetShape()->GetMassProperties();
				massProperties.ScaleToMass(mass);
				m_body->GetMotionProperties()->SetMassProperties(massProperties);
			}
		}
		else
		{
			JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
			bodyInterface.DeactivateBody(m_body->GetID());

			m_body->SetMotionType(JPH::EMotionType::Static);
		}
	}

	void JoltRigidBody3D::SetMassCenter(const Vector3f& center)
	{
		//m_body->GetMotionProperties()->set
	}

	void JoltRigidBody3D::SetPosition(const Vector3f& position)
	{
		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
		bodyInterface.SetPosition(m_body->GetID(), ToJolt(position), JPH::EActivation::Activate);
	}

	void JoltRigidBody3D::SetRotation(const Quaternionf& rotation)
	{
		JPH::BodyInterface& body_interface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
		body_interface.SetRotation(m_body->GetID(), ToJolt(rotation), JPH::EActivation::Activate);
	}

	void JoltRigidBody3D::TeleportTo(const Vector3f& position, const Quaternionf& rotation)
	{
		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
		bodyInterface.SetPositionAndRotation(m_body->GetID(), ToJolt(position), ToJolt(rotation), JPH::EActivation::Activate);
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
		body_interface.ActivateBody(m_body->GetID()); 
	}

	JoltRigidBody3D& JoltRigidBody3D::operator=(JoltRigidBody3D&& object) noexcept
	{
		Destroy();

		m_body          = object.m_body;
		m_bodyIndex     = object.m_bodyIndex;
		m_geom          = std::move(object.m_geom);
		m_world         = object.m_world;

		object.m_body = nullptr;
		object.m_bodyIndex = std::numeric_limits<UInt32>::max();

		return *this;
	}

	void JoltRigidBody3D::Destroy()
	{
		if (m_body)
		{
			JPH::BodyID bodyId = m_body->GetID();

			JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterface();
			bodyInterface.RemoveBody(bodyId);
			bodyInterface.DestroyBody(bodyId);
			m_body = nullptr;
		}

		m_geom.reset();
	}
}
