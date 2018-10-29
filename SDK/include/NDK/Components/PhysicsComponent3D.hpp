// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_COMPONENTS_PHYSICSCOMPONENT3D_HPP
#define NDK_COMPONENTS_PHYSICSCOMPONENT3D_HPP

#include <Nazara/Physics3D/RigidBody3D.hpp>
#include <NDK/Component.hpp>
#include <memory>

namespace Ndk
{
	class PhysicsComponent3D;

	using PhysicsComponent3DHandle = Nz::ObjectHandle<PhysicsComponent3D>;

	class NDK_API PhysicsComponent3D : public Component<PhysicsComponent3D>
	{
		friend class CollisionComponent3D;
		friend class PhysicsSystem3D;

		public:
			inline PhysicsComponent3D();
			PhysicsComponent3D(const PhysicsComponent3D& physics);
			~PhysicsComponent3D() = default;

			inline void AddForce(const Nz::Vector3f& force, Nz::CoordSys coordSys = Nz::CoordSys_Global);
			inline void AddForce(const Nz::Vector3f& force, const Nz::Vector3f& point, Nz::CoordSys coordSys = Nz::CoordSys_Global);
			inline void AddTorque(const Nz::Vector3f& torque, Nz::CoordSys coordSys = Nz::CoordSys_Global);

			inline void EnableAutoSleep(bool autoSleep);
			inline void EnableNodeSynchronization(bool nodeSynchronization);

			inline Nz::Boxf GetAABB() const;
			inline Nz::Vector3f GetAngularDamping() const;
			inline Nz::Vector3f GetAngularVelocity() const;
			inline float GetGravityFactor() const;
			inline float GetLinearDamping() const;
			inline Nz::Vector3f GetLinearVelocity() const;
			inline float GetMass() const;
			inline Nz::Vector3f GetMassCenter(Nz::CoordSys coordSys = Nz::CoordSys_Local) const;
			inline const Nz::Matrix4f& GetMatrix() const;
			inline Nz::Vector3f GetPosition() const;
			inline Nz::Quaternionf GetRotation() const;

			inline bool IsAutoSleepEnabled() const;
			inline bool IsMoveable() const;
			inline bool IsNodeSynchronizationEnabled() const;
			inline bool IsSleeping() const;

			inline void SetAngularDamping(const Nz::Vector3f& angularDamping);
			inline void SetAngularVelocity(const Nz::Vector3f& angularVelocity);
			inline void SetGravityFactor(float gravityFactor);
			inline void SetLinearDamping(float damping);
			inline void SetLinearVelocity(const Nz::Vector3f& velocity);
			inline void SetMass(float mass);
			inline void SetMassCenter(const Nz::Vector3f& center);
			inline void SetMaterial(const Nz::String& materialName);
			inline void SetMaterial(int materialIndex);
			inline void SetPosition(const Nz::Vector3f& position);
			inline void SetRotation(const Nz::Quaternionf& rotation);

			static ComponentIndex componentIndex;

		private:
			inline void ApplyPhysicsState(Nz::RigidBody3D& rigidBody) const;
			inline void CopyPhysicsState(const Nz::RigidBody3D& rigidBody);
			inline Nz::RigidBody3D* GetRigidBody();
			inline const Nz::RigidBody3D& GetRigidBody() const;

			void OnAttached() override;
			void OnComponentAttached(BaseComponent& component) override;
			void OnComponentDetached(BaseComponent& component) override;
			void OnDetached() override;
			void OnEntityDestruction() override;
			void OnEntityDisabled() override;
			void OnEntityEnabled() override;

			struct PendingPhysObjectStates
			{
				Nz::Vector3f angularDamping;
				Nz::Vector3f massCenter;
				bool autoSleep;
				bool valid = false;
				float gravityFactor;
				float linearDamping;
				float mass;
			};

			std::unique_ptr<Nz::RigidBody3D> m_object;
			PendingPhysObjectStates m_pendingStates;
			bool m_nodeSynchronizationEnabled;
	};
}

#include <NDK/Components/PhysicsComponent3D.inl>

#endif // NDK_COMPONENTS_PHYSICSCOMPONENT3D_HPP
