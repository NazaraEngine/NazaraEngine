// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_JOLTPHYSICS3D_JOLTCHARACTER_HPP
#define NAZARA_JOLTPHYSICS3D_JOLTCHARACTER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/JoltPhysics3D/Config.hpp>
#include <Nazara/JoltPhysics3D/JoltAbstractBody.hpp>
#include <Nazara/JoltPhysics3D/JoltPhysicsStepListener.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <memory>

namespace JPH
{
	class Character;
	class Body;
}

namespace Nz
{
	class JoltCharacterImpl;
	class JoltCollider3D;
	class JoltPhysWorld3D;

	class NAZARA_JOLTPHYSICS3D_API JoltCharacter : public JoltAbstractBody, public JoltPhysicsStepListener
	{
		friend JoltPhysWorld3D;

		public:
			struct Settings;

			JoltCharacter(JoltPhysWorld3D& physWorld, const Settings& settings);
			JoltCharacter(const JoltCharacter&) = delete;
			JoltCharacter(JoltCharacter&& character) noexcept;
			~JoltCharacter();

			inline void DisableSleeping();
			void EnableSleeping(bool enable);

			UInt32 GetBodyIndex() const override;
			inline const std::shared_ptr<JoltCollider3D>& GetCollider() const;
			Vector3f GetLinearVelocity() const;
			inline JoltPhysWorld3D& GetPhysWorld();
			inline const JoltPhysWorld3D& GetPhysWorld() const;
			Vector3f GetPosition() const;
			std::pair<Vector3f, Quaternionf> GetPositionAndRotation() const;
			Quaternionf GetRotation() const;
			Vector3f GetUp() const;

			bool IsOnGround() const;

			void SetFriction(float friction);
			inline void SetImpl(std::shared_ptr<JoltCharacterImpl> characterImpl);
			void SetLinearVelocity(const Vector3f& linearVel);
			void SetRotation(const Quaternionf& rotation);
			void SetUp(const Vector3f& up);

			void TeleportTo(const Vector3f& position, const Quaternionf& rotation);

			void WakeUp();

			JoltCharacter& operator=(const JoltCharacter&) = delete;
			JoltCharacter& operator=(JoltCharacter&& character) noexcept;

			struct Settings
			{
				std::shared_ptr<JoltCollider3D> collider;
				Quaternionf rotation = Quaternionf::Identity();
				Vector3f position = Vector3f::Zero();
			};

		protected:
			JoltCharacter();

			void Create(JoltPhysWorld3D& physWorld, const Settings& settings);
			void Destroy();

		private:
			void PostSimulate() override;
			void PreSimulate(float elapsedTime) override;

			std::shared_ptr<JoltCharacterImpl> m_impl;
			std::shared_ptr<JoltCollider3D> m_collider;
			std::unique_ptr<JPH::Character> m_character;
			MovablePtr<JoltPhysWorld3D> m_world;
			UInt32 m_bodyIndex;
	};

	class NAZARA_JOLTPHYSICS3D_API JoltCharacterImpl
	{
		public:
			JoltCharacterImpl() = default;
			JoltCharacterImpl(const JoltCharacterImpl&) = delete;
			JoltCharacterImpl(JoltCharacterImpl&&) = delete;
			virtual ~JoltCharacterImpl();

			virtual void PostSimulate(JoltCharacter& character);
			virtual void PreSimulate(JoltCharacter& character, float elapsedTime);

			JoltCharacterImpl& operator=(const JoltCharacterImpl&) = delete;
			JoltCharacterImpl& operator=(JoltCharacterImpl&&) = delete;
	};
}

#include <Nazara/JoltPhysics3D/JoltCharacter.inl>

#endif // NAZARA_JOLTPHYSICS3D_JOLTCHARACTER_HPP
