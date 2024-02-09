// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/PhysCharacter3D.hpp>
#include <Nazara/Physics3D/Collider3D.hpp>
#include <Nazara/Physics3D/JoltHelper.hpp>
#include <Nazara/Physics3D/PhysWorld3D.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	PhysCharacter3D::PhysCharacter3D() = default;

	PhysCharacter3D::PhysCharacter3D(PhysWorld3D& physWorld, const Settings& settings)
	{
		Create(physWorld, settings);
	}

	PhysCharacter3D::PhysCharacter3D(PhysCharacter3D&& character) noexcept :
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

	PhysCharacter3D::~PhysCharacter3D()
	{
		Destroy();
	}

	void PhysCharacter3D::EnableSleeping(bool enable)
	{
		const JPH::BodyLockInterfaceNoLock& bodyInterface = m_world->GetPhysicsSystem()->GetBodyLockInterfaceNoLock();
		JPH::BodyLockWrite bodyLock(bodyInterface, m_character->GetBodyID());
		if (!bodyLock.Succeeded())
			return;

		bodyLock.GetBody().SetAllowSleeping(enable);
	}

	UInt32 PhysCharacter3D::GetBodyIndex() const
	{
		return m_bodyIndex;
	}

	Vector3f PhysCharacter3D::GetLinearVelocity() const
	{
		return FromJolt(m_character->GetLinearVelocity(false));
	}

	Quaternionf PhysCharacter3D::GetRotation() const
	{
		return FromJolt(m_character->GetRotation(false));
	}

	Vector3f PhysCharacter3D::GetPosition() const
	{
		return FromJolt(m_character->GetPosition(false));
	}

	std::pair<Vector3f, Quaternionf> PhysCharacter3D::GetPositionAndRotation() const
	{
		JPH::Vec3 position;
		JPH::Quat rotation;
		m_character->GetPositionAndRotation(position, rotation, false);

		return { FromJolt(position), FromJolt(rotation) };
	}

	Vector3f PhysCharacter3D::GetUp() const
	{
		return FromJolt(m_character->GetUp());
	}

	bool PhysCharacter3D::IsOnGround() const
	{
		return m_character->GetGroundState() == JPH::Character::EGroundState::OnGround;
	}

	void PhysCharacter3D::SetFriction(float friction)
	{
		JPH::BodyInterface& bodyInterface = m_world->GetPhysicsSystem()->GetBodyInterfaceNoLock();
		bodyInterface.SetFriction(m_character->GetBodyID(), friction);
	}

	void PhysCharacter3D::SetLinearVelocity(const Vector3f& linearVel)
	{
		m_character->SetLinearVelocity(ToJolt(linearVel), false);
	}

	void PhysCharacter3D::SetRotation(const Quaternionf& rotation)
	{
		m_character->SetRotation(ToJolt(rotation), JPH::EActivation::Activate, false);
	}

	void PhysCharacter3D::SetUp(const Vector3f& up)
	{
		m_character->SetUp(ToJolt(up));
	}

	void PhysCharacter3D::TeleportTo(const Vector3f& position, const Quaternionf& rotation)
	{
		m_character->SetPositionAndRotation(ToJolt(position), ToJolt(rotation), JPH::EActivation::Activate, false);
	}

	void PhysCharacter3D::WakeUp()
	{
		m_character->Activate(false);
	}

	PhysCharacter3D& PhysCharacter3D::operator=(PhysCharacter3D&& character) noexcept
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

	void PhysCharacter3D::Create(PhysWorld3D& physWorld, const Settings& settings)
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

	void PhysCharacter3D::Destroy()
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

	void PhysCharacter3D::PostSimulate(float elapsedTime)
	{
		m_character->PostSimulation(0.05f);
		m_impl->PostSimulate(*this, elapsedTime);
	}

	void PhysCharacter3D::PreSimulate(float elapsedTime)
	{
		m_impl->PreSimulate(*this, elapsedTime);
	}


	PhysCharacter3DImpl::~PhysCharacter3DImpl() = default;

	void PhysCharacter3DImpl::PostSimulate(PhysCharacter3D& /*character*/, float /*elapsedTime*/)
	{
	}

	void PhysCharacter3DImpl::PreSimulate(PhysCharacter3D& /*character*/, float /*elapsedTime*/)
	{
	}
}
