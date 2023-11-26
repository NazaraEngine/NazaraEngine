// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/JoltRigidBody3D.hpp>
#include <Nazara/JoltPhysics3D/JoltHelper.hpp>
#include <Nazara/JoltPhysics3D/JoltPhysWorld3D.hpp>
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
	JoltRigidBody3D::JoltRigidBody3D(JoltRigidBody3D&& body) noexcept :
	m_geom(std::move(body.m_geom)),
	m_body(std::move(body.m_body)),
	m_world(std::move(body.m_world)),
	m_bodyIndex(body.m_bodyIndex),
	m_isSimulationEnabled(body.m_isSimulationEnabled),
	m_isTrigger(body.m_isTrigger)
	{
		body.m_bodyIndex = std::numeric_limits<UInt32>::max();

		if (m_body)
			m_body->SetUserData(SafeCast<UInt64>(reinterpret_cast<std::uintptr_t>(this)));
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
				m_body->AddForce(ToJolt(force));
				break;

			case CoordSys::Local:
				m_body->AddForce(m_body->GetRotation() * ToJolt(force));
				break;
		}
	}

	void JoltRigidBody3D::AddForce(const Vector3f& force, const Vector3f& point, CoordSys coordSys)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
				m_body->AddForce(ToJolt(force), ToJolt(point));
				break;

			case CoordSys::Local:
				m_body->AddForce(m_body->GetRotation() * ToJolt(force), ToJolt(ToWorld(point)));
				break;
		}
	}

	void JoltRigidBody3D::AddTorque(const Vector3f& torque, CoordSys coordSys)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
				m_body->AddTorque(ToJolt(torque));
				break;

			case CoordSys::Local:
				m_body->AddTorque(m_body->GetRotation() * ToJolt(torque));
				break;
		}
	}

	void JoltRigidBody3D::EnableSimulation(bool enable)
	{
		if (m_isSimulationEnabled == enable)
			return;

		if (enable)
			m_world->RegisterBody(m_body->GetID(), true, true);
		else
			m_world->UnregisterBody(m_body->GetID(), false, true);

		m_isSimulationEnabled = enable;
	}

	void JoltRigidBody3D::EnableSleeping(bool enable)
	{
		m_body->SetAllowSleeping(enable);
	}

	void JoltRigidBody3D::FallAsleep()
	{
		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterface();
		bodyInterface.DeactivateBody(m_body->GetID());
	}

	Boxf JoltRigidBody3D::GetAABB() const
	{
		const JPH::AABox& aabb = m_body->GetWorldSpaceBounds();
		return Boxf(FromJolt(aabb.mMin), FromJolt(aabb.GetSize()));
	}

	float JoltRigidBody3D::GetAngularDamping() const
	{
		if NAZARA_UNLIKELY(IsStatic())
			return 0.f;

		return m_body->GetMotionProperties()->GetAngularDamping();
	}

	Vector3f JoltRigidBody3D::GetAngularVelocity() const
	{
		return FromJolt(m_body->GetAngularVelocity());
	}

	float JoltRigidBody3D::GetLinearDamping() const
	{
		if NAZARA_UNLIKELY(IsStatic())
			return 0.f;

		return m_body->GetMotionProperties()->GetLinearDamping();
	}

	Vector3f JoltRigidBody3D::GetLinearVelocity() const
	{
		return FromJolt(m_body->GetLinearVelocity());
	}

	float JoltRigidBody3D::GetMass() const
	{
		if NAZARA_UNLIKELY(IsStatic())
			return 0.f;

		return 1.f / m_body->GetMotionProperties()->GetInverseMass();
	}

	Matrix4f JoltRigidBody3D::GetMatrix() const
	{
		return FromJolt(m_body->GetCenterOfMassTransform());
	}

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

	bool JoltRigidBody3D::IsSleeping() const
	{
		return m_body->IsActive();
	}

	bool JoltRigidBody3D::IsSleepingEnabled() const
	{
		return m_body->GetAllowSleeping();
	}

	bool JoltRigidBody3D::IsStatic() const
	{
		return m_body->GetMotionType() == JPH::EMotionType::Static;
	}

	void JoltRigidBody3D::SetAngularDamping(float angularDamping)
	{
		if NAZARA_UNLIKELY(IsStatic())
			return;

		m_body->GetMotionProperties()->SetAngularDamping(angularDamping);
	}

	void JoltRigidBody3D::SetAngularVelocity(const Vector3f& angularVelocity)
	{
		m_body->SetAngularVelocity(ToJolt(angularVelocity));
	}

	void JoltRigidBody3D::SetGeom(std::shared_ptr<JoltCollider3D> geom, bool recomputeInertia)
	{
		if (m_geom != geom)
		{
			float mass = GetMass();

			const JPH::Shape* shape;
			m_geom = std::move(geom);
			if (m_geom)
				shape = m_geom->GetShapeSettings()->Create().Get();
			else
			{
				m_body->SetIsSensor(true);
				shape = m_world->GetNullShape();
			}

			JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterface();
			bodyInterface.SetShape(m_body->GetID(), shape, false, (ShouldActivate()) ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
			if (recomputeInertia)
			{
				JPH::MassProperties massProperties = m_body->GetShape()->GetMassProperties();
				massProperties.ScaleToMass(mass);
				m_body->GetMotionProperties()->SetMassProperties(JPH::EAllowedDOFs::All, massProperties);
			}
		}
	}

	void JoltRigidBody3D::SetLinearDamping(float damping)
	{
		if NAZARA_UNLIKELY(IsStatic())
			return;

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
				m_body->GetMotionProperties()->SetMassProperties(JPH::EAllowedDOFs::All, massProperties);
			}
		}
		else
		{
			JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
			bodyInterface.DeactivateBody(m_body->GetID());

			m_body->SetMotionType(JPH::EMotionType::Kinematic);
		}
	}

	void JoltRigidBody3D::SetPosition(const Vector3f& position)
	{
		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
		bodyInterface.SetPosition(m_body->GetID(), ToJolt(position), (ShouldActivate()) ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
	}

	void JoltRigidBody3D::SetRotation(const Quaternionf& rotation)
	{
		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
		bodyInterface.SetRotation(m_body->GetID(), ToJolt(rotation), (ShouldActivate()) ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
	}

	void JoltRigidBody3D::TeleportTo(const Vector3f& position, const Quaternionf& rotation)
	{
		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
		bodyInterface.SetPositionAndRotation(m_body->GetID(), ToJolt(position), ToJolt(rotation), (ShouldActivate()) ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
	}

	Quaternionf JoltRigidBody3D::ToLocal(const Quaternionf& worldRotation)
	{
		return GetRotation().Conjugate() * worldRotation;
	}

	Vector3f JoltRigidBody3D::ToLocal(const Vector3f& worldPosition)
	{
		return FromJolt(m_body->GetInverseCenterOfMassTransform() * ToJolt(worldPosition));
	}

	Quaternionf JoltRigidBody3D::ToWorld(const Quaternionf& localRotation)
	{
		return GetRotation() * localRotation;
	}

	Vector3f JoltRigidBody3D::ToWorld(const Vector3f& localPosition)
	{
		return FromJolt(m_body->GetCenterOfMassTransform() * ToJolt(localPosition));
	}

	void JoltRigidBody3D::WakeUp()
	{
		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterface();
		bodyInterface.ActivateBody(m_body->GetID()); 
	}

	JoltRigidBody3D& JoltRigidBody3D::operator=(JoltRigidBody3D&& body) noexcept
	{
		Destroy();

		m_body                = std::move(body.m_body);
		m_bodyIndex           = body.m_bodyIndex;
		m_geom                = std::move(body.m_geom);
		m_world               = std::move(body.m_world);
		m_isSimulationEnabled = body.m_isSimulationEnabled;
		m_isTrigger           = body.m_isTrigger;

		body.m_bodyIndex = std::numeric_limits<UInt32>::max();

		if (m_body)
			m_body->SetUserData(SafeCast<UInt64>(reinterpret_cast<std::uintptr_t>(this)));

		return *this;
	}


	void JoltRigidBody3D::Create(JoltPhysWorld3D& world, const DynamicSettings& settings)
	{
		m_geom = settings.geom;
		m_isSimulationEnabled = settings.isSimulationEnabled;
		m_isTrigger = settings.isTrigger;
		m_world = &world;

		JPH::BodyCreationSettings creationSettings;
		BuildSettings(settings, creationSettings);

		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterface();
		m_body = bodyInterface.CreateBody(creationSettings);

		JPH::BodyID bodyId = m_body->GetID();
		m_bodyIndex = bodyId.GetIndex();

		if (settings.isSimulationEnabled)
			m_world->RegisterBody(bodyId, !settings.initiallySleeping, false);
	}

	void JoltRigidBody3D::Create(JoltPhysWorld3D& world, const StaticSettings& settings)
	{
		m_geom = settings.geom;
		m_isSimulationEnabled = settings.isSimulationEnabled;
		m_isTrigger = settings.isTrigger;
		m_world = &world;

		JPH::BodyCreationSettings creationSettings;
		BuildSettings(settings, creationSettings);

		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterface();
		m_body = bodyInterface.CreateBody(creationSettings);

		JPH::BodyID bodyId = m_body->GetID();
		m_bodyIndex = bodyId.GetIndex();

		if (settings.isSimulationEnabled)
			m_world->RegisterBody(bodyId, false, false); //< static bodies cannot be activated
	}

	void JoltRigidBody3D::Destroy(bool worldDestruction)
	{
		if (m_body)
		{
			m_world->UnregisterBody(m_body->GetID(), true, !worldDestruction);
			m_body = nullptr;
		}

		m_geom.reset();
	}

	void JoltRigidBody3D::BuildSettings(const DynamicSettings& settings, JPH::BodyCreationSettings& creationSettings)
	{
		BuildSettings(static_cast<const CommonSettings&>(settings), creationSettings);

		creationSettings.mAngularDamping = settings.angularDamping;
		creationSettings.mAngularVelocity = ToJolt(settings.angularVelocity);
		creationSettings.mLinearDamping = settings.linearDamping;
		creationSettings.mLinearVelocity = ToJolt(settings.linearVelocity);
		creationSettings.mFriction = settings.friction;
		creationSettings.mGravityFactor = settings.gravityFactor;
		creationSettings.mMaxAngularVelocity = settings.maxAngularVelocity;
		creationSettings.mMaxLinearVelocity = settings.maxLinearVelocity;
		creationSettings.mObjectLayer = 1;
		creationSettings.mRestitution = settings.restitution;

		creationSettings.mMotionType = (settings.mass > 0.f) ? JPH::EMotionType::Dynamic : JPH::EMotionType::Kinematic;

		float mass = settings.mass;
		if (mass <= 0.f)
			mass = 1.f;

		creationSettings.mMassPropertiesOverride = creationSettings.GetShape()->GetMassProperties();
		creationSettings.mMassPropertiesOverride.ScaleToMass(mass);
		creationSettings.mOverrideMassProperties = JPH::EOverrideMassProperties::MassAndInertiaProvided;


		switch (settings.motionQuality)
		{
			case JoltMotionQuality::Discrete:   creationSettings.mMotionQuality = JPH::EMotionQuality::Discrete;   break;
			case JoltMotionQuality::LinearCast: creationSettings.mMotionQuality = JPH::EMotionQuality::LinearCast; break;
		}
	}

	void JoltRigidBody3D::BuildSettings(const StaticSettings& settings, JPH::BodyCreationSettings& creationSettings)
	{
		BuildSettings(static_cast<const CommonSettings&>(settings), creationSettings);

		creationSettings.mMotionType = JPH::EMotionType::Static;
	}

	void JoltRigidBody3D::BuildSettings(const CommonSettings& settings, JPH::BodyCreationSettings& creationSettings)
	{
		if (settings.geom)
		{
			creationSettings.SetShapeSettings(settings.geom->GetShapeSettings());
			creationSettings.mIsSensor = settings.isTrigger;
		}
		else
		{
			creationSettings.SetShape(m_world->GetNullShape());
			creationSettings.mIsSensor = true; //< not the best solution but enough for now
		}

		creationSettings.mPosition = ToJolt(settings.position);
		creationSettings.mRotation = ToJolt(settings.rotation);
		creationSettings.mUserData = SafeCast<UInt64>(reinterpret_cast<std::uintptr_t>(this));
	}

	bool JoltRigidBody3D::ShouldActivate() const
	{
		return m_isSimulationEnabled && m_world->IsBodyRegistered(m_bodyIndex);
	}
}
