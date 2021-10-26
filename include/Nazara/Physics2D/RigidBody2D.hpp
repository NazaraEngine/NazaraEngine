// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RIGIDBODY2D_HPP
#define NAZARA_RIGIDBODY2D_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Math/Angle.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Physics2D/Config.hpp>
#include <Nazara/Physics2D/Collider2D.hpp>
#include <functional>
#include <limits>

struct cpBody;

namespace Nz
{
	class Arbiter2D;
	class PhysWorld2D;

	class NAZARA_PHYSICS2D_API RigidBody2D
	{
		public:
			using VelocityFunc = std::function<void(RigidBody2D& body2D, const Nz::Vector2f& gravity, float damping, float deltaTime)>;

			RigidBody2D(PhysWorld2D* world, float mass);
			RigidBody2D(PhysWorld2D* world, float mass, std::shared_ptr<Collider2D> geom);
			RigidBody2D(const RigidBody2D& object);
			RigidBody2D(RigidBody2D&& object) noexcept;
			~RigidBody2D();

			void AddForce(const Vector2f& force, CoordSys coordSys = CoordSys::Global);
			void AddForce(const Vector2f& force, const Vector2f& point, CoordSys coordSys = CoordSys::Global);
			void AddImpulse(const Vector2f& impulse, CoordSys coordSys = CoordSys::Global);
			void AddImpulse(const Vector2f& impulse, const Vector2f& point, CoordSys coordSys = CoordSys::Global);
			void AddTorque(const RadianAnglef& torque);

			bool ClosestPointQuery(const Nz::Vector2f& position, Nz::Vector2f* closestPoint = nullptr, float* closestDistance = nullptr) const;

			void EnableSimulation(bool simulation);

			void ForEachArbiter(std::function<void(Nz::Arbiter2D& /*arbiter*/)> callback);
			void ForceSleep();

			Rectf GetAABB() const;
			inline float GetAngularDamping() const;
			RadianAnglef GetAngularVelocity() const;
			NAZARA_DEPRECATED("Name error, please use GetMassCenter")
			inline Vector2f GetCenterOfGravity(CoordSys coordSys = CoordSys::Local) const;
			float GetElasticity(std::size_t shapeIndex = 0) const;
			float GetFriction(std::size_t shapeIndex = 0) const;
			const std::shared_ptr<Collider2D>& GetGeom() const;
			cpBody* GetHandle() const;
			float GetMass() const;
			Vector2f GetMassCenter(CoordSys coordSys = CoordSys::Local) const;
			float GetMomentOfInertia() const;
			Vector2f GetPosition() const;
			inline const Vector2f& GetPositionOffset() const;
			RadianAnglef GetRotation() const;
			inline std::size_t GetShapeCount() const;
			std::size_t GetShapeIndex(cpShape* shape) const;
			Vector2f GetSurfaceVelocity(std::size_t shapeIndex = 0) const;
			void* GetUserdata() const;
			Vector2f GetVelocity() const;
			const VelocityFunc& GetVelocityFunction() const;
			PhysWorld2D* GetWorld() const;

			bool IsKinematic() const;
			bool IsSimulationEnabled() const;
			bool IsSleeping() const;
			bool IsStatic() const;

			void ResetVelocityFunction();

			inline void SetAngularDamping(float angularDamping);
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
			void SetUserdata(void* ud);
			void SetVelocity(const Vector2f& velocity);
			void SetVelocityFunction(VelocityFunc velocityFunc);

			void UpdateVelocity(const Nz::Vector2f& gravity, float damping, float deltaTime);

			void Wakeup();

			RigidBody2D& operator=(const RigidBody2D& object);
			RigidBody2D& operator=(RigidBody2D&& object);

			NazaraSignal(OnRigidBody2DMove, RigidBody2D* /*oldPointer*/, RigidBody2D* /*newPointer*/);
			NazaraSignal(OnRigidBody2DRelease, RigidBody2D* /*rigidBody*/);

			static constexpr std::size_t InvalidShapeIndex = std::numeric_limits<std::size_t>::max();

		private:
			cpBody* Create(float mass = 1.f, float moment = 1.f);
			void Destroy();
			void RegisterToSpace();
			void UnregisterFromSpace();

			static void CopyBodyData(cpBody* from, cpBody* to);
			static void CopyShapeData(cpShape* from, cpShape* to);

			Vector2f m_positionOffset;
			VelocityFunc m_velocityFunc;
			std::vector<cpShape*> m_shapes;
			std::shared_ptr<Collider2D> m_geom;
			cpBody* m_handle;
			void* m_userData;
			PhysWorld2D* m_world;
			bool m_isRegistered;
			bool m_isSimulationEnabled;
			bool m_isStatic;
			float m_gravityFactor;
			float m_mass;
	};
}

#include <Nazara/Physics2D/RigidBody2D.inl>

#endif // NAZARA_RIGIDBODY2D_HPP
