// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS2D_RIGIDBODY2D_HPP
#define NAZARA_PHYSICS2D_RIGIDBODY2D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Math/Angle.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Physics2D/Collider2D.hpp>
#include <Nazara/Physics2D/Config.hpp>
#include <NazaraUtils/Signal.hpp>
#include <functional>
#include <limits>

struct cpBody;

namespace Nz
{
	class PhysArbiter2D;
	class PhysWorld2D;

	class NAZARA_PHYSICS2D_API RigidBody2D
	{
		public:
			struct DynamicSettings;
			struct StaticSettings;

			using VelocityFunc = std::function<void(RigidBody2D& body2D, const Vector2f& gravity, float damping, float deltaTime)>;

			inline RigidBody2D(PhysWorld2D& world, const DynamicSettings& settings);
			inline RigidBody2D(PhysWorld2D& world, const StaticSettings& settings);
			RigidBody2D(const RigidBody2D& object);
			RigidBody2D(RigidBody2D&& object) noexcept;
			inline ~RigidBody2D();

			inline void AddForce(const Vector2f& force, CoordSys coordSys = CoordSys::Global);
			void AddForce(const Vector2f& force, const Vector2f& point, CoordSys coordSys = CoordSys::Global);
			inline void AddImpulse(const Vector2f& impulse, CoordSys coordSys = CoordSys::Global);
			void AddImpulse(const Vector2f& impulse, const Vector2f& point, CoordSys coordSys = CoordSys::Global);
			void AddTorque(const RadianAnglef& torque);

			bool ClosestPointQuery(const Vector2f& position, Vector2f* closestPoint = nullptr, float* closestDistance = nullptr) const;

			void EnableSimulation(bool simulation);

			void ForEachArbiter(const FunctionRef<void(PhysArbiter2D& /*arbiter*/)>& callback);
			void ForceSleep();

			Rectf GetAABB() const;
			RadianAnglef GetAngularVelocity() const;
			inline UInt32 GetBodyIndex() const;
			float GetElasticity(std::size_t shapeIndex = 0) const;
			float GetFriction(std::size_t shapeIndex = 0) const;
			inline const std::shared_ptr<Collider2D>& GetGeom() const;
			inline cpBody* GetHandle() const;
			inline float GetMass() const;
			Vector2f GetMassCenter(CoordSys coordSys = CoordSys::Local) const;
			float GetMomentOfInertia() const;
			Vector2f GetPosition() const;
			inline const Vector2f& GetPositionOffset() const;
			RadianAnglef GetRotation() const;
			inline std::size_t GetShapeCount() const;
			inline std::size_t GetShapeIndex(cpShape* shape) const;
			Vector2f GetSurfaceVelocity(std::size_t shapeIndex = 0) const;
			Vector2f GetVelocity() const;
			inline const VelocityFunc& GetVelocityFunction() const;
			inline PhysWorld2D* GetWorld() const;

			inline bool IsKinematic() const;
			inline bool IsSimulationEnabled() const;
			bool IsSleeping() const;
			inline bool IsStatic() const;

			void ResetVelocityFunction();

			void SetAngularVelocity(const RadianAnglef& angularVelocity);
			void SetElasticity(float elasticity);
			void SetElasticity(std::size_t shapeIndex, float elasticity);
			void SetFriction(float friction);
			void SetFriction(std::size_t shapeIndex, float friction);
			void SetGeom(std::shared_ptr<Collider2D> geom, bool recomputeMoment = true, bool recomputeMassCenter = true);
			void SetMass(float mass, bool recomputeMoment = true);
			void SetMassCenter(const Vector2f& center, CoordSys coordSys = CoordSys::Local);
			void SetMomentOfInertia(float moment);
			void SetPosition(const Vector2f& position);
			void SetPositionOffset(const Vector2f& offset);
			void SetRotation(const RadianAnglef& rotation);
			void SetSurfaceVelocity(const Vector2f& surfaceVelocity);
			void SetSurfaceVelocity(std::size_t shapeIndex, const Vector2f& surfaceVelocity);
			void SetStatic(bool setStaticBody = true);
			void SetVelocity(const Vector2f& velocity);
			void SetVelocityFunction(VelocityFunc velocityFunc);

			void TeleportTo(const Vector2f& position, const RadianAnglef& rotation);

			RadianAnglef ToLocal(const RadianAnglef& worldRotation);
			Vector2f ToLocal(const Vector2f& worldPosition);
			RadianAnglef ToWorld(const RadianAnglef& localRotation);
			Vector2f ToWorld(const Vector2f& localPosition);

			void UpdateVelocity(const Vector2f& gravity, float damping, float deltaTime);

			void Wakeup();

			RigidBody2D& operator=(const RigidBody2D& object);
			RigidBody2D& operator=(RigidBody2D&& object) noexcept;

			static constexpr UInt32 InvalidBodyIndex = std::numeric_limits<UInt32>::max();
			static constexpr std::size_t InvalidShapeIndex = std::numeric_limits<std::size_t>::max();

			struct CommonSettings
			{
				std::shared_ptr<Collider2D> geom;
				RadianAnglef rotation = RadianAnglef::Zero();
				Vector2f position = Vector2f::Zero();
				bool initiallySleeping = false;
				bool isSimulationEnabled = true;
			};

			struct DynamicSettings : CommonSettings
			{
				DynamicSettings() = default;
				DynamicSettings(std::shared_ptr<Collider2D> collider, float mass_) :
				mass(mass_)
				{
					geom = std::move(collider);
				}

				RadianAnglef angularVelocity = RadianAnglef::Zero();
				Vector2f linearVelocity = Vector2f::Zero();
				float gravityFactor = 1.f;
				float mass = 1.f;
			};

			struct StaticSettings : CommonSettings
			{
				StaticSettings() = default;
				StaticSettings(std::shared_ptr<Collider2D> collider)
				{
					geom = std::move(collider);
				}
			};

		protected:
			RigidBody2D() = default;
			void Create(PhysWorld2D& world, const DynamicSettings& settings);
			void Create(PhysWorld2D& world, const StaticSettings& settings);
			void Destroy();

		private:
			void DestroyBody();
			void RegisterToSpace();
			void UnregisterFromSpace();

			static void CopyBodyData(cpBody* from, cpBody* to);
			static void CopyShapeData(cpShape* from, cpShape* to);

			std::vector<cpShape*> m_shapes;
			std::shared_ptr<Collider2D> m_geom;
			MovablePtr<cpBody> m_handle;
			MovablePtr<PhysWorld2D> m_world;
			UInt32 m_bodyIndex;
			Vector2f m_positionOffset;
			VelocityFunc m_velocityFunc;
			bool m_isRegistered;
			bool m_isSimulationEnabled;
			bool m_isStatic;
			float m_gravityFactor;
			float m_mass;
	};
}

#include <Nazara/Physics2D/RigidBody2D.inl>

#endif // NAZARA_PHYSICS2D_RIGIDBODY2D_HPP
