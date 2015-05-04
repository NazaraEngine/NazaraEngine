// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_COMPONENTS_PHYSICSCOMPONENT_HPP
#define NDK_COMPONENTS_PHYSICSCOMPONENT_HPP

#include <Nazara/Physics/PhysObject.hpp>
#include <NDK/Component.hpp>
#include <memory>

namespace Ndk
{
	class Entity;

	class NDK_API PhysicsComponent : public Component<PhysicsComponent>
	{
		friend class CollisionComponent;
		friend class PhysicsSystem;

		public:
			PhysicsComponent() = default;
			PhysicsComponent(const PhysicsComponent& physics);
			~PhysicsComponent() = default;

			void AddForce(const NzVector3f& force, nzCoordSys coordSys = nzCoordSys_Global);
			void AddForce(const NzVector3f& force, const NzVector3f& point, nzCoordSys coordSys = nzCoordSys_Global);
			void AddTorque(const NzVector3f& torque, nzCoordSys coordSys = nzCoordSys_Global);

			void EnableAutoSleep(bool autoSleep);

			NzBoxf GetAABB() const;
			NzVector3f GetAngularVelocity() const;
			float GetGravityFactor() const;
			float GetMass() const;
			NzVector3f GetMassCenter(nzCoordSys coordSys = nzCoordSys_Local) const;
			const NzMatrix4f& GetMatrix() const;
			NzVector3f GetPosition() const;
			NzQuaternionf GetRotation() const;
			NzVector3f GetVelocity() const;

			bool IsAutoSleepEnabled() const;
			bool IsMoveable() const;
			bool IsSleeping() const;

			void SetAngularVelocity(const NzVector3f& angularVelocity);
			void SetGravityFactor(float gravityFactor);
			void SetMass(float mass);
			void SetMassCenter(const NzVector3f& center);
			void SetPosition(const NzVector3f& position);
			void SetRotation(const NzQuaternionf& rotation);
			void SetVelocity(const NzVector3f& velocity);

			static ComponentIndex componentIndex;

		private:
			NzPhysObject& GetPhysObject();

			void OnAttached() override;
			void OnComponentAttached(BaseComponent& component) override;
			void OnComponentDetached(BaseComponent& component) override;
			void OnDetached() override;

			std::unique_ptr<NzPhysObject> m_object;
	};
}

#include <NDK/Components/PhysicsComponent.inl>

#endif // NDK_COMPONENTS_PHYSICSCOMPONENT_HPP
