// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - BulletPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BULLETPHYSICS3D_BULLETRIGIDBODY3D_HPP
#define NAZARA_BULLETPHYSICS3D_BULLETRIGIDBODY3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/BulletPhysics3D/BulletCollider3D.hpp>
#include <Nazara/BulletPhysics3D/Config.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <NazaraUtils/MovablePtr.hpp>

class btRigidBody;

namespace Nz
{
	class BulletPhysWorld3D;

	class NAZARA_BULLETPHYSICS3D_API BulletRigidBody3D
	{
		public:
			BulletRigidBody3D(BulletPhysWorld3D* world, const Matrix4f& mat = Matrix4f::Identity());
			BulletRigidBody3D(BulletPhysWorld3D* world, std::shared_ptr<BulletCollider3D> geom, const Matrix4f& mat = Matrix4f::Identity());
			BulletRigidBody3D(const BulletRigidBody3D& object) = delete;
			BulletRigidBody3D(BulletRigidBody3D&& object) noexcept;
			~BulletRigidBody3D();

			void AddForce(const Vector3f& force, CoordSys coordSys = CoordSys::Global);
			void AddForce(const Vector3f& force, const Vector3f& point, CoordSys coordSys = CoordSys::Global);
			void AddTorque(const Vector3f& torque, CoordSys coordSys = CoordSys::Global);

			inline void DisableSleeping();
			void EnableSleeping(bool enable);

			void FallAsleep();

			Boxf GetAABB() const;
			float GetAngularDamping() const;
			Vector3f GetAngularVelocity() const;
			inline const std::shared_ptr<BulletCollider3D>& GetGeom() const;
			float GetLinearDamping() const;
			Vector3f GetLinearVelocity() const;
			float GetMass() const;
			Vector3f GetMassCenter(CoordSys coordSys = CoordSys::Local) const;
			Matrix4f GetMatrix() const;
			Vector3f GetPosition() const;
			inline btRigidBody* GetRigidBody() const;
			Quaternionf GetRotation() const;
			inline std::size_t GetUniqueIndex() const;
			inline BulletPhysWorld3D* GetWorld() const;

			bool IsSimulationEnabled() const;
			bool IsSleeping() const;
			bool IsSleepingEnabled() const;

			void SetAngularDamping(float angularDamping);
			void SetAngularVelocity(const Vector3f& angularVelocity);
			void SetGeom(std::shared_ptr<BulletCollider3D> geom, bool recomputeInertia = true);
			void SetLinearDamping(float damping);
			void SetLinearVelocity(const Vector3f& velocity);
			void SetMass(float mass);
			void SetMassCenter(const Vector3f& center);
			void SetPosition(const Vector3f& position);
			void SetPositionAndRotation(const Vector3f& position, const Quaternionf& rotation);
			void SetRotation(const Quaternionf& rotation);

			Quaternionf ToLocal(const Quaternionf& worldRotation);
			Vector3f ToLocal(const Vector3f& worldPosition);
			Quaternionf ToWorld(const Quaternionf& localRotation);
			Vector3f ToWorld(const Vector3f& localPosition);

			void WakeUp();

			BulletRigidBody3D& operator=(const BulletRigidBody3D& object) = delete;
			BulletRigidBody3D& operator=(BulletRigidBody3D&& object) noexcept;

		protected:
			void Destroy();

		private:
			std::shared_ptr<BulletCollider3D> m_geom;
			std::size_t m_bodyPoolIndex;
			btRigidBody* m_body;
			BulletPhysWorld3D* m_world;
	};
}

#include <Nazara/BulletPhysics3D/BulletRigidBody3D.inl>

#endif // NAZARA_BULLETPHYSICS3D_BULLETRIGIDBODY3D_HPP
