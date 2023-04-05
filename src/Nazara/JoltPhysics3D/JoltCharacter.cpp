// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/JoltCharacter.hpp>
#include <Nazara/JoltPhysics3D/JoltCollider3D.hpp>
#include <Nazara/JoltPhysics3D/JoltHelper.hpp>
#include <Nazara/JoltPhysics3D/JoltPhysWorld3D.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	JoltCharacter::JoltCharacter(JoltPhysWorld3D& physWorld, std::shared_ptr<JoltCollider3D> collider, const Vector3f& position, const Quaternionf& rotation) :
	m_collider(std::move(collider)),
	m_physicsWorld(physWorld)
	{
		auto shapeResult = m_collider->GetShapeSettings()->Create();
		if (!shapeResult.IsValid())
			throw std::runtime_error("invalid shape");

		JPH::CharacterSettings settings;
		settings.mShape = shapeResult.Get();
		settings.mLayer = 1;

		m_character = std::make_unique<JPH::Character>(&settings, ToJolt(position), ToJolt(rotation), 0, m_physicsWorld.GetPhysicsSystem());
		m_character->AddToPhysicsSystem();

		m_physicsWorld.RegisterCharacter(this);
	}

	JoltCharacter::~JoltCharacter()
	{
		m_character->RemoveFromPhysicsSystem();

		m_physicsWorld.UnregisterCharacter(this);
	}

	void JoltCharacter::EnableSleeping(bool enable)
	{
		const JPH::BodyLockInterfaceNoLock& bodyInterface = m_physicsWorld.GetPhysicsSystem()->GetBodyLockInterfaceNoLock();
		JPH::BodyLockWrite bodyLock(bodyInterface, m_character->GetBodyID());
		if (!bodyLock.Succeeded())
			return;

		bodyLock.GetBody().SetAllowSleeping(enable);
	}

	Vector3f JoltCharacter::GetLinearVelocity() const
	{
		return FromJolt(m_character->GetLinearVelocity(false));
	}

	Quaternionf JoltCharacter::GetRotation() const
	{
		return FromJolt(m_character->GetRotation(false));
	}

	Vector3f JoltCharacter::GetPosition() const
	{
		return FromJolt(m_character->GetPosition(false));
	}

	std::pair<Vector3f, Quaternionf> JoltCharacter::GetPositionAndRotation() const
	{
		JPH::Vec3 position;
		JPH::Quat rotation;
		m_character->GetPositionAndRotation(position, rotation, false);

		return { FromJolt(position), FromJolt(rotation) };
	}

	Vector3f JoltCharacter::GetUp() const
	{
		return FromJolt(m_character->GetUp());
	}

	bool JoltCharacter::IsOnGround() const
	{
		return m_character->IsSupported();
	}

	void JoltCharacter::SetFriction(float friction)
	{
		JPH::BodyInterface& bodyInterface = m_physicsWorld.GetPhysicsSystem()->GetBodyInterfaceNoLock();
		bodyInterface.SetFriction(m_character->GetBodyID(), friction);
	}

	void JoltCharacter::SetLinearVelocity(const Vector3f& linearVel)
	{
		m_character->SetLinearVelocity(ToJolt(linearVel), false);
	}

	void JoltCharacter::SetRotation(const Quaternionf& rotation)
	{
		m_character->SetRotation(ToJolt(rotation), JPH::EActivation::Activate, false);
	}

	void JoltCharacter::SetUp(const Vector3f& up)
	{
		m_character->SetUp(ToJolt(up));
	}

	void JoltCharacter::WakeUp()
	{
		m_character->Activate(false);
	}

	void JoltCharacter::PreSimulate(float /*elapsedTime*/)
	{
	}

	void JoltCharacter::PostSimulate()
	{
		m_character->PostSimulation(0.05f);
	}
}
