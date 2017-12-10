// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RIGIDBODY3D_HPP
#define NAZARA_RIGIDBODY3D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics3D/Config.hpp>
#include <Nazara/Physics3D/Collider3D.hpp>

class NewtonBody;

namespace Nz
{
	class PhysWorld3D;

	class NAZARA_PHYSICS3D_API RigidBody3D
	{
		public:
			RigidBody3D(PhysWorld3D* world, const Matrix4f& mat = Matrix4f::Identity());
			RigidBody3D(PhysWorld3D* world, Collider3DRef geom, const Matrix4f& mat = Matrix4f::Identity());
			RigidBody3D(const RigidBody3D& object);
			RigidBody3D(RigidBody3D&& object);
			~RigidBody3D();

			void AddForce(const Vector3f& force, CoordSys coordSys = CoordSys_Global);
			void AddForce(const Vector3f& force, const Vector3f& point, CoordSys coordSys = CoordSys_Global);
			void AddTorque(const Vector3f& torque, CoordSys coordSys = CoordSys_Global);

			void EnableAutoSleep(bool autoSleep);

			Boxf GetAABB() const;
			Vector3f GetAngularDamping() const;
			Vector3f GetAngularVelocity() const;
			const Collider3DRef& GetGeom() const;
			float GetGravityFactor() const;
			NewtonBody* GetHandle() const;
			float GetLinearDamping() const;
			Vector3f GetLinearVelocity() const;
			float GetMass() const;
			Vector3f GetMassCenter(CoordSys coordSys = CoordSys_Local) const;
			const Matrix4f& GetMatrix() const;
			Vector3f GetPosition() const;
			Quaternionf GetRotation() const;
			PhysWorld3D* GetWorld() const;

			bool IsAutoSleepEnabled() const;
			bool IsMoveable() const;
			bool IsSleeping() const;

			void SetAngularDamping(const Nz::Vector3f& angularDamping);
			void SetAngularVelocity(const Vector3f& angularVelocity);
			void SetGeom(Collider3DRef geom);
			void SetGravityFactor(float gravityFactor);
			void SetLinearDamping(float damping);
			void SetLinearVelocity(const Vector3f& velocity);
			void SetMass(float mass);
			void SetMassCenter(const Vector3f& center);
			void SetPosition(const Vector3f& position);
			void SetRotation(const Quaternionf& rotation);

			RigidBody3D& operator=(const RigidBody3D& object);
			RigidBody3D& operator=(RigidBody3D&& object);

		private:
			void UpdateBody();
			static void ForceAndTorqueCallback(const NewtonBody* body, float timeStep, int threadIndex);
			static void TransformCallback(const NewtonBody* body, const float* matrix, int threadIndex);

			Collider3DRef m_geom;
			Matrix4f m_matrix;
			Vector3f m_forceAccumulator;
			Vector3f m_torqueAccumulator;
			NewtonBody* m_body;
			PhysWorld3D* m_world;
			float m_gravityFactor;
			float m_mass;
	};
}

#endif // NAZARA_RIGIDBODY3D_HPP
