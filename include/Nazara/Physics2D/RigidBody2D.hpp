// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RIGIDBODY2D_HPP
#define NAZARA_RIGIDBODY2D_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Physics2D/Config.hpp>
#include <Nazara/Physics2D/Collider2D.hpp>
#include <limits>

struct cpBody;

namespace Nz
{
	class PhysWorld2D;

	class NAZARA_PHYSICS2D_API RigidBody2D
	{
		public:
			RigidBody2D(PhysWorld2D* world, float mass);
			RigidBody2D(PhysWorld2D* world, float mass, Collider2DRef geom);
			RigidBody2D(const RigidBody2D& object);
			RigidBody2D(RigidBody2D&& object);
			~RigidBody2D();

			void AddForce(const Vector2f& force, CoordSys coordSys = CoordSys_Global);
			void AddForce(const Vector2f& force, const Vector2f& point, CoordSys coordSys = CoordSys_Global);
			void AddImpulse(const Vector2f& impulse, CoordSys coordSys = CoordSys_Global);
			void AddImpulse(const Vector2f& impulse, const Vector2f& point, CoordSys coordSys = CoordSys_Global);
			void AddTorque(float torque);

			bool ClosestPointQuery(const Nz::Vector2f& position, Nz::Vector2f* closestPoint = nullptr, float* closestDistance = nullptr) const;

			void EnableSimulation(bool simulation);

			Rectf GetAABB() const;
			inline float GetAngularDamping() const;
			float GetAngularVelocity() const;
			NAZARA_DEPRECATED("Name error, please use GetMassCenter")
			inline Vector2f GetCenterOfGravity(CoordSys coordSys = CoordSys_Local) const;
			const Collider2DRef& GetGeom() const;
			cpBody* GetHandle() const;
			float GetMass() const;
			Vector2f GetMassCenter(CoordSys coordSys = CoordSys_Local) const;
			float GetMomentOfInertia() const;
			Vector2f GetPosition() const;
			float GetRotation() const;
			std::size_t GetShapeIndex(cpShape* shape) const;
			void* GetUserdata() const;
			Vector2f GetVelocity() const;
			PhysWorld2D* GetWorld() const;

			bool IsKinematic() const;
			bool IsSimulationEnabled() const;
			bool IsSleeping() const;
			bool IsStatic() const;

			inline void SetAngularDamping(float angularDamping);
			void SetAngularVelocity(float angularVelocity);
			void SetGeom(Collider2DRef geom, bool recomputeMoment = true);
			void SetMass(float mass, bool recomputeMoment = true);
			void SetMassCenter(const Vector2f& center, CoordSys coordSys = CoordSys_Local);
			void SetMomentOfInertia(float moment);
			void SetPosition(const Vector2f& position);
			void SetRotation(float rotation);
			void SetStatic(bool setStaticBody = true);
			void SetUserdata(void* ud);
			void SetVelocity(const Vector2f& velocity);

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

			std::vector<cpShape*> m_shapes;
			Collider2DRef m_geom;
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
