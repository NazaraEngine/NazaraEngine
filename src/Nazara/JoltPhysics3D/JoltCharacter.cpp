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

		m_character = std::make_unique<JPH::Character>(&settings, ToJolt(position), ToJolt(rotation), 0, m_physicsWorld.GetPhysicsSystem());
		m_character->AddToPhysicsSystem();

		m_physicsWorld.RegisterCharacter(this);
	}

	JoltCharacter::~JoltCharacter()
	{
		m_character->RemoveFromPhysicsSystem();

		m_physicsWorld.UnregisterCharacter(this);
	}

	Vector3f JoltCharacter::GetLinearVelocity() const
	{
		return FromJolt(m_character->GetLinearVelocity());
	}

	Quaternionf JoltCharacter::GetRotation() const
	{
		return FromJolt(m_character->GetRotation());
	}

	Vector3f JoltCharacter::GetPosition() const
	{
		return FromJolt(m_character->GetPosition());
	}

	std::pair<Vector3f, Quaternionf> JoltCharacter::GetPositionAndRotation() const
	{
		JPH::Vec3 position;
		JPH::Quat rotation;
		m_character->GetPositionAndRotation(position, rotation);

		return { FromJolt(position), FromJolt(rotation) };
	}

	bool JoltCharacter::IsOnGround() const
	{
		return m_character->IsSupported();
	}

	void JoltCharacter::SetLinearVelocity(const Vector3f& linearVel)
	{
		m_character->SetLinearVelocity(ToJolt(linearVel));
	}

	void JoltCharacter::PostSimulate()
	{
		m_character->PostSimulation(0.05f);
	}
}
