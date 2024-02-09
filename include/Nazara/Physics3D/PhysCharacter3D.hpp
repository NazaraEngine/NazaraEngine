// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_PHYSCHARACTER3D_HPP
#define NAZARA_PHYSICS3D_PHYSCHARACTER3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics3D/Config.hpp>
#include <Nazara/Physics3D/Physics3DBody.hpp>
#include <Nazara/Physics3D/Physiscs3DStepListener.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <memory>

namespace JPH
{
	class Character;
	class Body;
}

namespace Nz
{
	class PhysCharacter3DImpl;
	class Collider3D;
	class PhysWorld3D;

	class NAZARA_PHYSICS3D_API PhysCharacter3D : public Physics3DBody, public Physiscs3DStepListener
	{
		friend PhysWorld3D;

		public:
			struct Settings;

			PhysCharacter3D(PhysWorld3D& physWorld, const Settings& settings);
			PhysCharacter3D(const PhysCharacter3D&) = delete;
			PhysCharacter3D(PhysCharacter3D&& character) noexcept;
			~PhysCharacter3D();

			inline void DisableSleeping();
			void EnableSleeping(bool enable);

			UInt32 GetBodyIndex() const override;
			inline const std::shared_ptr<Collider3D>& GetCollider() const;
			Vector3f GetLinearVelocity() const;
			inline PhysWorld3D& GetPhysWorld();
			inline const PhysWorld3D& GetPhysWorld() const;
			Vector3f GetPosition() const;
			std::pair<Vector3f, Quaternionf> GetPositionAndRotation() const;
			Quaternionf GetRotation() const;
			Vector3f GetUp() const;

			bool IsOnGround() const;

			void SetFriction(float friction);
			inline void SetImpl(std::shared_ptr<PhysCharacter3DImpl> characterImpl);
			void SetLinearVelocity(const Vector3f& linearVel);
			void SetRotation(const Quaternionf& rotation);
			void SetUp(const Vector3f& up);

			void TeleportTo(const Vector3f& position, const Quaternionf& rotation);

			void WakeUp();

			PhysCharacter3D& operator=(const PhysCharacter3D&) = delete;
			PhysCharacter3D& operator=(PhysCharacter3D&& character) noexcept;

			struct Settings
			{
				std::shared_ptr<Collider3D> collider;
				Quaternionf rotation = Quaternionf::Identity();
				Vector3f position = Vector3f::Zero();
			};

		protected:
			PhysCharacter3D();

			void Create(PhysWorld3D& physWorld, const Settings& settings);
			void Destroy();

		private:
			void PostSimulate(float elapsedTime) override;
			void PreSimulate(float elapsedTime) override;

			std::shared_ptr<PhysCharacter3DImpl> m_impl;
			std::shared_ptr<Collider3D> m_collider;
			std::unique_ptr<JPH::Character> m_character;
			MovablePtr<PhysWorld3D> m_world;
			UInt32 m_bodyIndex;
	};

	class NAZARA_PHYSICS3D_API PhysCharacter3DImpl
	{
		public:
			PhysCharacter3DImpl() = default;
			PhysCharacter3DImpl(const PhysCharacter3DImpl&) = delete;
			PhysCharacter3DImpl(PhysCharacter3DImpl&&) = delete;
			virtual ~PhysCharacter3DImpl();

			virtual void PostSimulate(PhysCharacter3D& character, float elapsedTime);
			virtual void PreSimulate(PhysCharacter3D& character, float elapsedTime);

			PhysCharacter3DImpl& operator=(const PhysCharacter3DImpl&) = delete;
			PhysCharacter3DImpl& operator=(PhysCharacter3DImpl&&) = delete;
	};
}

#include <Nazara/Physics3D/PhysCharacter3D.inl>

#endif // NAZARA_PHYSICS3D_PHYSCHARACTER3D_HPP
