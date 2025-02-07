// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_RIGIDBODY3D_HPP
#define NAZARA_PHYSICS3D_RIGIDBODY3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics3D/Collider3D.hpp>
#include <Nazara/Physics3D/Export.hpp>
#include <Nazara/Physics3D/PhysBody3D.hpp>
#include <NazaraUtils/MovablePtr.hpp>

namespace JPH
{
	class Body;
	class BodyCreationSettings;
	class Shape;
}

namespace Nz
{
	class PhysWorld3D;

	class NAZARA_PHYSICS3D_API RigidBody3D : public PhysBody3D
	{
		public:
			struct DynamicSettings;
			struct StaticSettings;
			using Settings = std::variant<DynamicSettings, StaticSettings>;

			inline RigidBody3D(PhysWorld3D& world, const DynamicSettings& settings);
			inline RigidBody3D(PhysWorld3D& world, const StaticSettings& settings);
			inline RigidBody3D(PhysWorld3D& world, const Settings& settings);
			RigidBody3D(const RigidBody3D& object) = delete;
			RigidBody3D(RigidBody3D&& object) noexcept;
			~RigidBody3D();

			void AddAngularImpulse(const Vector3f& angularImpulse, CoordSys coordSys = CoordSys::Global);
			void AddForce(const Vector3f& force, CoordSys coordSys = CoordSys::Global);
			void AddForce(const Vector3f& force, const Vector3f& point, CoordSys coordSys = CoordSys::Global);
			void AddImpulse(const Vector3f& impulse, CoordSys coordSys = CoordSys::Global);
			void AddImpulse(const Vector3f& impulse, const Vector3f& point, CoordSys coordSys = CoordSys::Global);
			void AddLinearVelocity(const Vector3f& linearVelocity);
			void AddTorque(const Vector3f& torque, CoordSys coordSys = CoordSys::Global);

			bool ApplyBuoyancyImpulse(const Vector3f& surfacePosition, const Vector3f& surfaceNormal, float buoyancy, float linearDrag, float angularDrag, const Vector3f& fluidVelocity, const Vector3f& gravity, float deltaTime);

			inline void DisableSimulation();
			inline void DisableSleeping();
			void EnableSimulation(bool enable);
			void EnableSleeping(bool enable);

			void FallAsleep();

			Boxf GetAABB() const;
			float GetAngularDamping() const;
			Vector3f GetAngularVelocity() const;
			inline JPH::Body* GetBody();
			inline const JPH::Body* GetBody() const;
			UInt32 GetBodyIndex() const override;
			inline const std::shared_ptr<Collider3D>& GetCollider() const;
			std::pair<Vector3f, Vector3f> GetLinearAndAngularVelocity() const;
			float GetLinearDamping() const;
			Vector3f GetLinearVelocity() const;
			float GetMass() const;
			Matrix4f GetMatrix() const;
			PhysObjectLayer3D GetObjectLayer() const override;
			Vector3f GetPosition() const;
			std::pair<Vector3f, Quaternionf> GetPositionAndRotation() const;
			Quaternionf GetRotation() const;
			Settings GetSettings() const;
			inline PhysWorld3D& GetWorld() const;

			bool IsDynamic() const;
			bool IsKinematic() const;
			inline bool IsSimulationEnabled() const;
			bool IsSleeping() const;
			bool IsSleepingEnabled() const;
			bool IsStatic() const;
			bool IsTrigger() const override;

			void SetAngularDamping(float angularDamping);
			void SetAngularVelocity(const Vector3f& angularVelocity);
			void SetCollider(std::shared_ptr<Collider3D> collider, bool recomputeInertia = true);
			void SetLinearAndAngularVelocity(const Vector3f& linearVelocity, const Vector3f& angularVelocity);
			void SetLinearDamping(float damping);
			void SetLinearVelocity(const Vector3f& velocity);
			void SetMass(float mass, bool recomputeInertia = true);
			void SetObjectLayer(PhysObjectLayer3D objectLayer);
			void SetPosition(const Vector3f& position);
			void SetRotation(const Quaternionf& rotation);

			void TeleportTo(const Vector3f& position, const Quaternionf& rotation);

			Quaternionf ToLocal(const Quaternionf& worldRotation);
			Vector3f ToLocal(const Vector3f& worldPosition);
			Quaternionf ToWorld(const Quaternionf& localRotation);
			Vector3f ToWorld(const Vector3f& localPosition);

			void WakeUp();

			RigidBody3D& operator=(const RigidBody3D& object) = delete;
			RigidBody3D& operator=(RigidBody3D&& object) noexcept;

			struct CommonSettings
			{
				std::shared_ptr<Collider3D> collider;
				PhysObjectLayer3D objectLayer = 0;
				Quaternionf rotation = Quaternionf::Identity();
				Vector3f position = Vector3f::Zero();
				bool initiallySleeping = false;
				bool isSimulationEnabled = true;
				bool isTrigger = false;
			};

			struct DynamicSettings : CommonSettings
			{
				DynamicSettings() = default;
				DynamicSettings(std::shared_ptr<Collider3D> collider_, float mass_) :
				mass(mass_)
				{
					collider = std::move(collider_);
				}

				// Default values from Jolt
				PhysMotionQuality3D motionQuality = PhysMotionQuality3D::Discrete;
				Vector3f angularVelocity = Vector3f::Zero();
				Vector3f linearVelocity = Vector3f::Zero();
				bool allowSleeping = true;
				float angularDamping = 0.05f;
				float friction = 0.2f;
				float gravityFactor = 1.f;
				float linearDamping = 0.05f;
				float mass = 1.f;
				float maxAngularVelocity = 0.25f * Pi<float>() * 60.f;
				float maxLinearVelocity = 500.f;
				float restitution = 0.f;
			};

			struct StaticSettings : CommonSettings
			{
				StaticSettings() = default;
				StaticSettings(std::shared_ptr<Collider3D> collider_)
				{
					collider = std::move(collider_);
				}
			};

		protected:
			RigidBody3D() = default;
			void Create(PhysWorld3D& world, const DynamicSettings& settings);
			void Create(PhysWorld3D& world, const StaticSettings& settings);
			void Create(PhysWorld3D& world, const Settings& settings);
			void Destroy(bool worldDestruction = false);

		private:
			void BuildSettings(const CommonSettings& settings, JPH::BodyCreationSettings& creationSettings);
			void BuildSettings(const DynamicSettings& settings, JPH::BodyCreationSettings& creationSettings);
			void BuildSettings(const StaticSettings& settings, JPH::BodyCreationSettings& creationSettings);

			bool ShouldActivate() const;

			std::shared_ptr<Collider3D> m_collider;
			MovablePtr<JPH::Body> m_body;
			MovablePtr<PhysWorld3D> m_world;
			UInt32 m_bodyIndex;
			bool m_isSimulationEnabled;
			bool m_isTrigger;
	};
}

#include <Nazara/Physics3D/RigidBody3D.inl>

#endif // NAZARA_PHYSICS3D_RIGIDBODY3D_HPP
