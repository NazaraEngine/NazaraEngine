// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_RIGIDBODY3D_HPP
#define NAZARA_PHYSICS3D_RIGIDBODY3D_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics3D/Collider3D.hpp>
#include <Nazara/Physics3D/Config.hpp>

class NewtonBody;

namespace Nz
{
	class PhysWorld3D;

	class NAZARA_PHYSICS3D_API RigidBody3D
	{
		public:
			RigidBody3D(PhysWorld3D* world, const Matrix4f& mat = Matrix4f::Identity());
			RigidBody3D(PhysWorld3D* world, std::shared_ptr<Collider3D> geom, const Matrix4f& mat = Matrix4f::Identity());
			RigidBody3D(const RigidBody3D& object);
			RigidBody3D(RigidBody3D&& object) noexcept;
			~RigidBody3D();

			void AddForce(const Vector3f& force, CoordSys coordSys = CoordSys::Global);
			void AddForce(const Vector3f& force, const Vector3f& point, CoordSys coordSys = CoordSys::Global);
			void AddTorque(const Vector3f& torque, CoordSys coordSys = CoordSys::Global);

			void EnableAutoSleep(bool autoSleep);
			void EnableSimulation(bool simulation);

			Boxf GetAABB() const;
			Vector3f GetAngularDamping() const;
			Vector3f GetAngularVelocity() const;
			const std::shared_ptr<Collider3D>& GetGeom() const;
			float GetGravityFactor() const;
			NewtonBody* GetHandle() const;
			float GetLinearDamping() const;
			Vector3f GetLinearVelocity() const;
			float GetMass() const;
			Vector3f GetMassCenter(CoordSys coordSys = CoordSys::Local) const;
			int GetMaterial() const;
			Matrix4f GetMatrix() const;
			Vector3f GetPosition() const;
			Quaternionf GetRotation() const;
			void* GetUserdata() const;
			PhysWorld3D* GetWorld() const;

			bool IsAutoSleepEnabled() const;
			bool IsMoveable() const;
			bool IsSimulationEnabled() const;
			bool IsSleeping() const;

			void SetAngularDamping(const Vector3f& angularDamping);
			void SetAngularVelocity(const Vector3f& angularVelocity);
			void SetGeom(std::shared_ptr<Collider3D> geom);
			void SetGravityFactor(float gravityFactor);
			void SetLinearDamping(float damping);
			void SetLinearVelocity(const Vector3f& velocity);
			void SetMass(float mass);
			void SetMassCenter(const Vector3f& center);
			void SetMaterial(const std::string& materialName);
			void SetMaterial(int materialIndex);
			void SetPosition(const Vector3f& position);
			void SetRotation(const Quaternionf& rotation);
			void SetUserdata(void* ud);

			RigidBody3D& operator=(const RigidBody3D& object);
			RigidBody3D& operator=(RigidBody3D&& object) noexcept;

		protected:
			void Destroy();

		private:
			void UpdateBody(const Matrix4f& transformMatrix);
			static void ForceAndTorqueCallback(const NewtonBody* body, float timeStep, int threadIndex);

			std::shared_ptr<Collider3D> m_geom;
			MovablePtr<NewtonBody> m_body;
			Vector3f m_forceAccumulator;
			Vector3f m_torqueAccumulator;
			PhysWorld3D* m_world;
			void* m_userdata;
			float m_gravityFactor;
			float m_mass;
	};
}

#endif // NAZARA_PHYSICS3D_RIGIDBODY3D_HPP
