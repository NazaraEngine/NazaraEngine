// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_JOLTPHYSICS3D_JOLTCHARACTER_HPP
#define NAZARA_JOLTPHYSICS3D_JOLTCHARACTER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/JoltPhysics3D/Config.hpp>
#include <memory>

namespace JPH
{
	class Character;
	class Body;
}

namespace Nz
{
	class JoltCollider3D;
	class JoltPhysWorld3D;

	class NAZARA_JOLTPHYSICS3D_API JoltCharacter
	{
		friend JoltPhysWorld3D;

		public:
			JoltCharacter(JoltPhysWorld3D& physWorld, std::shared_ptr<JoltCollider3D> collider, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity());
			JoltCharacter(const JoltCharacter&) = delete;
			JoltCharacter(JoltCharacter&&) = delete;
			~JoltCharacter();

			inline void DisableSleeping();
			void EnableSleeping(bool enable);

			Vector3f GetLinearVelocity() const;
			Quaternionf GetRotation() const;
			Vector3f GetPosition() const;
			std::pair<Vector3f, Quaternionf> GetPositionAndRotation() const;
			Vector3f GetUp() const;

			bool IsOnGround() const;

			void SetFriction(float friction);
			void SetLinearVelocity(const Vector3f& linearVel);
			void SetRotation(const Quaternionf& rotation);
			void SetUp(const Vector3f& up);

			void WakeUp();

			JoltCharacter& operator=(const JoltCharacter&) = delete;
			JoltCharacter& operator=(JoltCharacter&&) = delete;

		protected:
			virtual void PreSimulate(float elapsedTime);
			virtual void PostSimulate();

		private:
			std::shared_ptr<JoltCollider3D> m_collider;
			std::unique_ptr<JPH::Character> m_character;
			JoltPhysWorld3D& m_physicsWorld;
	};
}

#include <Nazara/JoltPhysics3D/JoltCharacter.inl>

#endif // NAZARA_JOLTPHYSICS3D_JOLTCHARACTER_HPP
