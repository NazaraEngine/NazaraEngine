// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Physics3D/RigidBody3D.hpp>
#include <Nazara/Physics3D/JoltHelper.hpp>
#include <Nazara/Physics3D/PhysWorld3D.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <cmath>

namespace Nz
{
	RigidBody3D::RigidBody3D(RigidBody3D&& body) noexcept :
	m_collider(std::move(body.m_collider)),
	m_body(std::move(body.m_body)),
	m_world(std::move(body.m_world)),
	m_bodyIndex(body.m_bodyIndex),
	m_isSimulationEnabled(body.m_isSimulationEnabled),
	m_isTrigger(body.m_isTrigger)
	{
		body.m_bodyIndex = std::numeric_limits<UInt32>::max();

		if (m_body)
			m_body->SetUserData(PointerToInteger<UInt64>(this));
	}

	RigidBody3D::~RigidBody3D()
	{
		Destroy();
	}

	void RigidBody3D::AddForce(const Vector3f& force, CoordSys coordSys)
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

	void RigidBody3D::AddForce(const Vector3f& force, const Vector3f& point, CoordSys coordSys)
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

	void RigidBody3D::AddTorque(const Vector3f& torque, CoordSys coordSys)
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

	void RigidBody3D::EnableSimulation(bool enable)
	{
		if (m_isSimulationEnabled == enable)
			return;

		if (enable)
			m_world->RegisterBody(m_body->GetID(), true, true);
		else
			m_world->UnregisterBody(m_body->GetID(), false, true);

		m_isSimulationEnabled = enable;
	}

	void RigidBody3D::EnableSleeping(bool enable)
	{
		m_body->SetAllowSleeping(enable);
	}

	void RigidBody3D::FallAsleep()
	{
		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterface();
		bodyInterface.DeactivateBody(m_body->GetID());
	}

	Boxf RigidBody3D::GetAABB() const
	{
		const JPH::AABox& aabb = m_body->GetWorldSpaceBounds();
		return Boxf(FromJolt(aabb.mMin), FromJolt(aabb.GetSize()));
	}

	float RigidBody3D::GetAngularDamping() const
	{
		if NAZARA_UNLIKELY(IsStatic())
			return 0.f;

		return m_body->GetMotionProperties()->GetAngularDamping();
	}

	Vector3f RigidBody3D::GetAngularVelocity() const
	{
		return FromJolt(m_body->GetAngularVelocity());
	}

	UInt32 RigidBody3D::GetBodyIndex() const
	{
		return m_bodyIndex;
	}

	std::pair<Vector3f, Vector3f> RigidBody3D::GetLinearAndAngularVelocity() const
	{
		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();

		JPH::Vec3 angularVel;
		JPH::Vec3 linearVel;
		bodyInterface.GetLinearAndAngularVelocity(m_body->GetID(), linearVel, angularVel);

		return { FromJolt(linearVel), FromJolt(angularVel) };
	}

	float RigidBody3D::GetLinearDamping() const
	{
		if NAZARA_UNLIKELY(IsStatic())
			return 0.f;

		return m_body->GetMotionProperties()->GetLinearDamping();
	}

	Vector3f RigidBody3D::GetLinearVelocity() const
	{
		return FromJolt(m_body->GetLinearVelocity());
	}

	float RigidBody3D::GetMass() const
	{
		if NAZARA_UNLIKELY(IsStatic())
			return 0.f;

		return 1.f / m_body->GetMotionProperties()->GetInverseMass();
	}

	Matrix4f RigidBody3D::GetMatrix() const
	{
		return FromJolt(m_body->GetCenterOfMassTransform());
	}

	PhysObjectLayer3D RigidBody3D::GetObjectLayer() const
	{
		return m_body->GetObjectLayer();
	}

	Vector3f RigidBody3D::GetPosition() const
	{
		return FromJolt(m_body->GetPosition());
	}

	std::pair<Vector3f, Quaternionf> RigidBody3D::GetPositionAndRotation() const
	{
		JPH::Vec3 position = m_body->GetPosition();
		JPH::Quat rotation = m_body->GetRotation();

		return { FromJolt(position), FromJolt(rotation) };
	}

	Quaternionf RigidBody3D::GetRotation() const
	{
		return FromJolt(m_body->GetRotation());
	}

	bool RigidBody3D::IsDynamic() const
	{
		return m_body->IsDynamic();
	}

	bool RigidBody3D::IsKinematic() const
	{
		return m_body->IsKinematic();
	}

	bool RigidBody3D::IsSleeping() const
	{
		return !m_body->IsActive();
	}

	bool RigidBody3D::IsSleepingEnabled() const
	{
		return m_body->GetAllowSleeping();
	}

	bool RigidBody3D::IsStatic() const
	{
		return m_body->GetMotionType() == JPH::EMotionType::Static;
	}

	void RigidBody3D::SetAngularDamping(float angularDamping)
	{
		if NAZARA_UNLIKELY(IsStatic())
			return;

		m_body->GetMotionProperties()->SetAngularDamping(angularDamping);
	}

	void RigidBody3D::SetAngularVelocity(const Vector3f& angularVelocity)
	{
		m_body->SetAngularVelocity(ToJolt(angularVelocity));
	}

	void RigidBody3D::SetCollider(std::shared_ptr<Collider3D> collider, bool recomputeInertia)
	{
		if (!collider)
			collider = EmptyCollider3D::Get();

		if (m_collider != collider)
		{
			float mass;
			if (m_body->IsDynamic())
				mass = GetMass();

			m_collider = std::move(collider);

			const JPH::Shape* shape = m_collider->GetShapeSettings()->Create().Get();

			JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterface();
			bodyInterface.SetShape(m_body->GetID(), shape, false, (ShouldActivate()) ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
			if (recomputeInertia && m_body->IsDynamic())
			{
				JPH::MassProperties massProperties = m_body->GetShape()->GetMassProperties();
				massProperties.ScaleToMass(mass);
				m_body->GetMotionProperties()->SetMassProperties(JPH::EAllowedDOFs::All, massProperties);
			}
		}
	}

	void RigidBody3D::SetLinearAndAngularVelocity(const Vector3f& linearVelocity, const Vector3f& angularVelocity)
	{
		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
		bodyInterface.SetLinearAndAngularVelocity(m_body->GetID(), ToJolt(linearVelocity), ToJolt(angularVelocity));
	}

	void RigidBody3D::SetLinearDamping(float damping)
	{
		if NAZARA_UNLIKELY(IsStatic())
			return;

		m_body->GetMotionProperties()->SetLinearDamping(damping);
	}

	void RigidBody3D::SetLinearVelocity(const Vector3f& velocity)
	{
		m_body->SetLinearVelocity(ToJolt(velocity));
	}

	void RigidBody3D::SetMass(float mass, bool recomputeInertia)
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

	void RigidBody3D::SetObjectLayer(PhysObjectLayer3D objectLayer)
	{
		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
		bodyInterface.SetObjectLayer(m_body->GetID(), objectLayer);
	}

	void RigidBody3D::SetPosition(const Vector3f& position)
	{
		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
		bodyInterface.SetPosition(m_body->GetID(), ToJolt(position), (ShouldActivate()) ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
	}

	void RigidBody3D::SetRotation(const Quaternionf& rotation)
	{
		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
		bodyInterface.SetRotation(m_body->GetID(), ToJolt(rotation), (ShouldActivate()) ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
	}

	void RigidBody3D::TeleportTo(const Vector3f& position, const Quaternionf& rotation)
	{
		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
		bodyInterface.SetPositionAndRotation(m_body->GetID(), ToJolt(position), ToJolt(rotation), (ShouldActivate()) ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
	}

	Quaternionf RigidBody3D::ToLocal(const Quaternionf& worldRotation)
	{
		return GetRotation().Conjugate() * worldRotation;
	}

	Vector3f RigidBody3D::ToLocal(const Vector3f& worldPosition)
	{
		return FromJolt(m_body->GetInverseCenterOfMassTransform() * ToJolt(worldPosition));
	}

	Quaternionf RigidBody3D::ToWorld(const Quaternionf& localRotation)
	{
		return GetRotation() * localRotation;
	}

	Vector3f RigidBody3D::ToWorld(const Vector3f& localPosition)
	{
		return FromJolt(m_body->GetCenterOfMassTransform() * ToJolt(localPosition));
	}

	void RigidBody3D::WakeUp()
	{
		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
		if (bodyInterface.IsAdded(m_body->GetID()))
			bodyInterface.ActivateBody(m_body->GetID());
	}

	RigidBody3D& RigidBody3D::operator=(RigidBody3D&& body) noexcept
	{
		Destroy();

		m_body                = std::move(body.m_body);
		m_bodyIndex           = body.m_bodyIndex;
		m_collider                = std::move(body.m_collider);
		m_world               = std::move(body.m_world);
		m_isSimulationEnabled = body.m_isSimulationEnabled;
		m_isTrigger           = body.m_isTrigger;

		body.m_bodyIndex = std::numeric_limits<UInt32>::max();

		if (m_body)
			m_body->SetUserData(PointerToInteger<UInt64>(this));

		return *this;
	}


	void RigidBody3D::Create(PhysWorld3D& world, const DynamicSettings& settings)
	{
		m_collider = (settings.collider) ? settings.collider : EmptyCollider3D::Get();
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

	void RigidBody3D::Create(PhysWorld3D& world, const StaticSettings& settings)
	{
		m_collider = settings.collider;
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

	void RigidBody3D::Destroy(bool worldDestruction)
	{
		if (m_body)
		{
			m_world->UnregisterBody(m_body->GetID(), true, !worldDestruction);
			m_body = nullptr;
		}

		m_collider.reset();
	}

	void RigidBody3D::BuildSettings(const DynamicSettings& settings, JPH::BodyCreationSettings& creationSettings)
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
			case PhysMotionQuality3D::Discrete:   creationSettings.mMotionQuality = JPH::EMotionQuality::Discrete;   break;
			case PhysMotionQuality3D::LinearCast: creationSettings.mMotionQuality = JPH::EMotionQuality::LinearCast; break;
		}
	}

	void RigidBody3D::BuildSettings(const StaticSettings& settings, JPH::BodyCreationSettings& creationSettings)
	{
		BuildSettings(static_cast<const CommonSettings&>(settings), creationSettings);

		creationSettings.mMotionType = JPH::EMotionType::Static;
	}

	void RigidBody3D::BuildSettings(const CommonSettings& settings, JPH::BodyCreationSettings& creationSettings)
	{
		if (settings.collider)
			creationSettings.SetShapeSettings(settings.collider->GetShapeSettings());
		else
			creationSettings.SetShapeSettings(EmptyCollider3D::Get()->GetShapeSettings());

		creationSettings.mObjectLayer = settings.objectLayer;
		creationSettings.mPosition = ToJolt(settings.position);
		creationSettings.mRotation = ToJolt(settings.rotation);
		creationSettings.mUserData = PointerToInteger<UInt64>(this);
	}

	bool RigidBody3D::ShouldActivate() const
	{
		return m_isSimulationEnabled && m_world->IsBodyRegistered(m_bodyIndex);
	}
}
