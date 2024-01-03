// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/JoltCharacter.hpp>
#include <Nazara/JoltPhysics3D/JoltCollider3D.hpp>
#include <Nazara/JoltPhysics3D/JoltHelper.hpp>
#include <Nazara/JoltPhysics3D/JoltPhysWorld3D.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	JoltCharacter::JoltCharacter() = default;

	JoltCharacter::JoltCharacter(JoltPhysWorld3D& physWorld, const Settings& settings)
	{
		Create(physWorld, settings);
	}

	JoltCharacter::JoltCharacter(JoltCharacter&& character) noexcept :
	m_impl(std::move(character.m_impl)),
	m_collider(std::move(character.m_collider)),
	m_character(std::move(character.m_character)),
	m_world(std::move(character.m_world)),
	m_bodyIndex(character.m_bodyIndex)
	{
		character.m_bodyIndex = std::numeric_limits<UInt32>::max();

		if (m_character)
		{
			JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
			bodyInterface.SetUserData(m_character->GetBodyID(), SafeCast<UInt64>(BitCast<std::uintptr_t>(this)));
		}

		if (m_world)
		{
			m_world->UnregisterStepListener(&character);
			m_world->RegisterStepListener(this);
		}
	}

	JoltCharacter::~JoltCharacter()
	{
		Destroy();
	}

	void JoltCharacter::EnableSleeping(bool enable)
	{
		const JPH::BodyLockInterfaceNoLock& bodyInterface = m_world->GetPhysicsSystem()->GetBodyLockInterfaceNoLock();
		JPH::BodyLockWrite bodyLock(bodyInterface, m_character->GetBodyID());
		if (!bodyLock.Succeeded())
			return;

		bodyLock.GetBody().SetAllowSleeping(enable);
	}

	UInt32 JoltCharacter::GetBodyIndex() const
	{
		return m_bodyIndex;
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
		return m_character->GetGroundState() == JPH::Character::EGroundState::OnGround;
	}

	void JoltCharacter::SetFriction(float friction)
	{
		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
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

	void JoltCharacter::TeleportTo(const Vector3f& position, const Quaternionf& rotation)
	{
		m_character->SetPositionAndRotation(ToJolt(position), ToJolt(rotation), JPH::EActivation::Activate, false);
	}

	void JoltCharacter::WakeUp()
	{
		m_character->Activate(false);
	}
	
	JoltCharacter& JoltCharacter::operator=(JoltCharacter&& character) noexcept
	{
		Destroy();

		m_impl = std::move(character.m_impl);
		m_collider = std::move(character.m_collider);
		m_character = std::move(character.m_character);
		m_bodyIndex = character.m_bodyIndex;
		m_world = std::move(character.m_world);

		if (m_world)
		{
			m_world->UnregisterStepListener(&character);
			m_world->RegisterStepListener(this);
		}

		character.m_bodyIndex = std::numeric_limits<UInt32>::max();

		if (m_character)
		{
			JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
			bodyInterface.SetUserData(m_character->GetBodyID(), SafeCast<UInt64>(BitCast<std::uintptr_t>(this)));
		}

		return *this;
	}

	void JoltCharacter::Create(JoltPhysWorld3D& physWorld, const Settings& settings)
	{
		m_collider = settings.collider;
		m_impl = physWorld.GetDefaultCharacterImpl();
		m_world = &physWorld;

		auto shapeResult = m_collider->GetShapeSettings()->Create();
		if (!shapeResult.IsValid())
			throw std::runtime_error("invalid shape");

		JPH::CharacterSettings characterSettings;
		characterSettings.mShape = shapeResult.Get();
		characterSettings.mLayer = 1;

		m_character = std::make_unique<JPH::Character>(&characterSettings, ToJolt(settings.position), ToJolt(settings.rotation), 0, m_world->GetPhysicsSystem());
		m_character->AddToPhysicsSystem();

		m_bodyIndex = m_character->GetBodyID().GetIndex();

		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
		bodyInterface.SetUserData(m_character->GetBodyID(), SafeCast<UInt64>(BitCast<std::uintptr_t>(this)));

		m_world->RegisterStepListener(this);
	}

	void JoltCharacter::Destroy()
	{
		if (m_character)
		{
			m_character->RemoveFromPhysicsSystem();
			m_character = nullptr;
		}

		if (m_world)
		{
			m_world->UnregisterStepListener(this);
			m_world = nullptr;
		}

		m_collider.reset();
	}

	void JoltCharacter::PostSimulate(float elapsedTime)
	{
		m_character->PostSimulation(0.05f);
		m_impl->PostSimulate(*this, elapsedTime);
	}

	void JoltCharacter::PreSimulate(float elapsedTime)
	{
		m_impl->PreSimulate(*this, elapsedTime);
	}


	JoltCharacterImpl::~JoltCharacterImpl() = default;

	void JoltCharacterImpl::PostSimulate(JoltCharacter& /*character*/, float /*elapsedTime*/)
	{
	}

	void JoltCharacterImpl::PreSimulate(JoltCharacter& /*character*/, float /*elapsedTime*/)
	{
	}
}
