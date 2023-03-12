// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_RIGIDBODY3D_HPP
#define NAZARA_PHYSICS3D_RIGIDBODY3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics3D/Collider3D.hpp>
#include <Nazara/Physics3D/Config.hpp>
#include <NazaraUtils/MovablePtr.hpp>

class btRigidBody;

namespace Nz
{
	class PhysWorld3D;

	class NAZARA_PHYSICS3D_API RigidBody3D
	{
		public:
			RigidBody3D(PhysWorld3D* world, const Matrix4f& mat = Matrix4f::Identity());
			RigidBody3D(PhysWorld3D* world, std::shared_ptr<Collider3D> geom, const Matrix4f& mat = Matrix4f::Identity());
			RigidBody3D(const RigidBody3D& object) = delete;
			RigidBody3D(RigidBody3D&& object) noexcept;
			~RigidBody3D();

			void AddForce(const Vector3f& force, CoordSys coordSys = CoordSys::Global);
			void AddForce(const Vector3f& force, const Vector3f& point, CoordSys coordSys = CoordSys::Global);
			void AddTorque(const Vector3f& torque, CoordSys coordSys = CoordSys::Global);

			inline void DisableSleeping();
			void EnableSleeping(bool enable);

			void FallAsleep();

			Boxf GetAABB() const;
			float GetAngularDamping() const;
			Vector3f GetAngularVelocity() const;
			const std::shared_ptr<Collider3D>& GetGeom() const;
			float GetLinearDamping() const;
			Vector3f GetLinearVelocity() const;
			float GetMass() const;
			Vector3f GetMassCenter(CoordSys coordSys = CoordSys::Local) const;
			Matrix4f GetMatrix() const;
			Vector3f GetPosition() const;
			btRigidBody* GetRigidBody() const;
			Quaternionf GetRotation() const;
			PhysWorld3D* GetWorld() const;

			bool IsSimulationEnabled() const;
			bool IsSleeping() const;
			bool IsSleepingEnabled() const;

			void SetAngularDamping(float angularDamping);
			void SetAngularVelocity(const Vector3f& angularVelocity);
			void SetGeom(std::shared_ptr<Collider3D> geom, bool recomputeInertia = true);
			void SetLinearDamping(float damping);
			void SetLinearVelocity(const Vector3f& velocity);
			void SetMass(float mass);
			void SetMassCenter(const Vector3f& center);
			void SetPosition(const Vector3f& position);
			void SetRotation(const Quaternionf& rotation);

			void WakeUp();

			RigidBody3D& operator=(const RigidBody3D& object) = delete;
			RigidBody3D& operator=(RigidBody3D&& object) noexcept;

		protected:
			void Destroy();

		private:
			std::shared_ptr<Collider3D> m_geom;
			std::size_t m_bodyPoolIndex;
			btRigidBody* m_body;
			PhysWorld3D* m_world;
	};
}

#include <Nazara/Physics3D/RigidBody3D.inl>

#endif // NAZARA_PHYSICS3D_RIGIDBODY3D_HPP
