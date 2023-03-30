// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_JOLTPHYSICS3D_JOLTRIGIDBODY3D_HPP
#define NAZARA_JOLTPHYSICS3D_JOLTRIGIDBODY3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/JoltPhysics3D/JoltCollider3D.hpp>
#include <Nazara/JoltPhysics3D/Config.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <NazaraUtils/MovablePtr.hpp>

namespace JPH
{
	class Body;
}

namespace Nz
{
	class JoltPhysWorld3D;

	class NAZARA_JOLTPHYSICS3D_API JoltRigidBody3D
	{
		public:
			JoltRigidBody3D(JoltPhysWorld3D* world, const Matrix4f& mat = Matrix4f::Identity());
			JoltRigidBody3D(JoltPhysWorld3D* world, std::shared_ptr<JoltCollider3D> geom, const Matrix4f& mat = Matrix4f::Identity());
			JoltRigidBody3D(const JoltRigidBody3D& object) = delete;
			JoltRigidBody3D(JoltRigidBody3D&& object) noexcept;
			~JoltRigidBody3D();

			void AddForce(const Vector3f& force, CoordSys coordSys = CoordSys::Global);
			void AddForce(const Vector3f& force, const Vector3f& point, CoordSys coordSys = CoordSys::Global);
			void AddTorque(const Vector3f& torque, CoordSys coordSys = CoordSys::Global);

			inline void DisableSleeping();
			void EnableSleeping(bool enable);

			void FallAsleep();

			Boxf GetAABB() const;
			float GetAngularDamping() const;
			Vector3f GetAngularVelocity() const;
			inline JPH::Body* GetBody();
			inline const JPH::Body* GetBody() const;
			inline UInt32 GetBodyIndex() const;
			inline const std::shared_ptr<JoltCollider3D>& GetGeom() const;
			float GetLinearDamping() const;
			Vector3f GetLinearVelocity() const;
			float GetMass() const;
			Vector3f GetMassCenter(CoordSys coordSys = CoordSys::Local) const;
			Matrix4f GetMatrix() const;
			Vector3f GetPosition() const;
			std::pair<Vector3f, Quaternionf> GetPositionAndRotation() const;
			Quaternionf GetRotation() const;
			inline JoltPhysWorld3D* GetWorld() const;

			bool IsSimulationEnabled() const;
			bool IsSleeping() const;
			bool IsSleepingEnabled() const;

			void SetAngularDamping(float angularDamping);
			void SetAngularVelocity(const Vector3f& angularVelocity);
			void SetGeom(std::shared_ptr<JoltCollider3D> geom, bool recomputeInertia = true);
			void SetLinearDamping(float damping);
			void SetLinearVelocity(const Vector3f& velocity);
			void SetMass(float mass, bool recomputeInertia = true);
			void SetMassCenter(const Vector3f& center);
			void SetPosition(const Vector3f& position);
			void SetRotation(const Quaternionf& rotation);

			void TeleportTo(const Vector3f& position, const Quaternionf& rotation);

			Quaternionf ToLocal(const Quaternionf& worldRotation);
			Vector3f ToLocal(const Vector3f& worldPosition);
			Quaternionf ToWorld(const Quaternionf& localRotation);
			Vector3f ToWorld(const Vector3f& localPosition);

			void WakeUp();

			JoltRigidBody3D& operator=(const JoltRigidBody3D& object) = delete;
			JoltRigidBody3D& operator=(JoltRigidBody3D&& object) noexcept;

		protected:
			void Destroy();

		private:
			std::shared_ptr<JoltCollider3D> m_geom;
			JPH::Body* m_body;
			JoltPhysWorld3D* m_world;
			UInt32 m_bodyIndex;
	};
}

#include <Nazara/JoltPhysics3D/JoltRigidBody3D.inl>

#endif // NAZARA_JOLTPHYSICS3D_JOLTRIGIDBODY3D_HPP
