// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RIGIDBODY2D_HPP
#define NAZARA_RIGIDBODY2D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics2D/Config.hpp>
#include <Nazara/Physics2D/Collider2D.hpp>

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
			void AddTorque(float torque);

			Rectf GetAABB() const;
			float GetAngularVelocity() const;
			Vector2f GetCenterOfGravity(CoordSys coordSys = CoordSys_Local) const;
			const Collider2DRef& GetGeom() const;
			float GetGravityFactor() const;
			cpBody* GetHandle() const;
			float GetMass() const;
			Vector2f GetPosition() const;
			float GetRotation() const;
			Vector2f GetVelocity() const;

			bool IsMoveable() const;
			bool IsSleeping() const;

			void SetAngularVelocity(float angularVelocity);
			void SetGravityFactor(float gravityFactor);
			void SetMass(float mass);
			void SetMassCenter(const Vector2f& center);
			void SetPosition(const Vector2f& position);
			void SetRotation(float rotation);
			void SetVelocity(const Vector2f& velocity);

			RigidBody2D& operator=(const RigidBody2D& object);
			RigidBody2D& operator=(RigidBody2D&& object);

		private:
			void Destroy();
			void SetGeom(Collider2DRef geom);

			std::vector<cpShape*> m_shapes;
			Collider2DRef m_geom;
			cpBody* m_handle;
			PhysWorld2D* m_world;
			float m_gravityFactor;
			float m_mass;
	};
}

#endif // NAZARA_RIGIDBODY3D_HPP
