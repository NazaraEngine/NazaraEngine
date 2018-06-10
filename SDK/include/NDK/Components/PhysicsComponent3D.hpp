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
	class NDK_API PhysicsComponent3D : public Component<PhysicsComponent3D>
	{
		friend class CollisionComponent3D;
		friend class PhysicsSystem3D;

		public:
			inline PhysicsComponent3D();
			PhysicsComponent3D(const PhysicsComponent3D& physics);
			~PhysicsComponent3D() = default;

			void AddForce(const Nz::Vector3f& force, Nz::CoordSys coordSys = Nz::CoordSys_Global);
			void AddForce(const Nz::Vector3f& force, const Nz::Vector3f& point, Nz::CoordSys coordSys = Nz::CoordSys_Global);
			void AddTorque(const Nz::Vector3f& torque, Nz::CoordSys coordSys = Nz::CoordSys_Global);

			void EnableAutoSleep(bool autoSleep);
			void EnableNodeSynchronization(bool nodeSynchronization);

			Nz::Boxf GetAABB() const;
			Nz::Vector3f GetAngularDamping() const;
			Nz::Vector3f GetAngularVelocity() const;
			float GetGravityFactor() const;
			float GetLinearDamping() const;
			Nz::Vector3f GetLinearVelocity() const;
			float GetMass() const;
			Nz::Vector3f GetMassCenter(Nz::CoordSys coordSys = Nz::CoordSys_Local) const;
			const Nz::Matrix4f& GetMatrix() const;
			Nz::Vector3f GetPosition() const;
			Nz::Quaternionf GetRotation() const;

			bool IsAutoSleepEnabled() const;
			bool IsMoveable() const;
			bool IsNodeSynchronizationEnabled() const;
			bool IsSleeping() const;

			void SetAngularDamping(const Nz::Vector3f& angularDamping);
			void SetAngularVelocity(const Nz::Vector3f& angularVelocity);
			void SetGravityFactor(float gravityFactor);
			void SetLinearDamping(float damping);
			void SetLinearVelocity(const Nz::Vector3f& velocity);
			void SetMass(float mass);
			void SetMassCenter(const Nz::Vector3f& center);
			void SetMaterial(const Nz::String& materialName);
			void SetMaterial(int materialIndex);
			void SetPosition(const Nz::Vector3f& position);
			void SetRotation(const Nz::Quaternionf& rotation);

			static ComponentIndex componentIndex;

		private:
			void ApplyPhysicsState(Nz::RigidBody3D& rigidBody) const;
			void CopyPhysicsState(const Nz::RigidBody3D& rigidBody);
			Nz::RigidBody3D* GetRigidBody();
			const Nz::RigidBody3D& GetRigidBody() const;

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
