// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_COMPONENTS_PHYSICSCOMPONENT2D_HPP
#define NDK_COMPONENTS_PHYSICSCOMPONENT2D_HPP

#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <NDK/Component.hpp>
#include <memory>

namespace Ndk
{
	class NDK_API PhysicsComponent2D : public Component<PhysicsComponent2D>
	{
		friend class CollisionComponent2D;
		friend class PhysicsSystem2D;
		friend class ConstraintComponent2D;

		public:
			PhysicsComponent2D() = default;
			PhysicsComponent2D(const PhysicsComponent2D& physics);
			~PhysicsComponent2D() = default;
			
			inline void AddForce(const Nz::Vector2f& force, Nz::CoordSys coordSys = Nz::CoordSys_Global);
			inline void AddForce(const Nz::Vector2f& force, const Nz::Vector2f& point, Nz::CoordSys coordSys = Nz::CoordSys_Global);
			inline void AddImpulse(const Nz::Vector2f& impulse, Nz::CoordSys coordSys = Nz::CoordSys_Global);
			inline void AddImpulse(const Nz::Vector2f& impulse, const Nz::Vector2f& point, Nz::CoordSys coordSys = Nz::CoordSys_Global);
			inline void AddTorque(float torque);

			inline bool ClosestPointQuery(const Nz::Vector2f& position, Nz::Vector2f* closestPoint, float* closestDistance) const;

			inline Nz::Rectf GetAABB() const;
			inline float GetAngularDamping() const;
			inline float GetAngularVelocity() const;
			NAZARA_DEPRECATED("Name error, please use GetMassCenter")
			inline Nz::Vector2f GetCenterOfGravity(Nz::CoordSys coordSys = Nz::CoordSys_Local) const;
			inline float GetMass() const;
			inline Nz::Vector2f GetMassCenter(Nz::CoordSys coordSys = Nz::CoordSys_Local) const;
			inline float GetMomentOfInertia() const;
			inline Nz::Vector2f GetPosition() const;
			inline float GetRotation() const;
			inline Nz::Vector2f GetVelocity() const;

			inline bool IsSleeping() const;

			inline void SetAngularDamping(float angularDamping);
			inline void SetAngularVelocity(float angularVelocity);
			inline void SetMass(float mass);
			inline void SetMassCenter(const Nz::Vector2f& center, Nz::CoordSys coordSys = Nz::CoordSys_Local);
			inline void SetMomentOfInertia(float moment);
			inline void SetPosition(const Nz::Vector2f& position);
			inline void SetRotation(float rotation);
			inline void SetVelocity(const Nz::Vector2f& velocity);

			static ComponentIndex componentIndex;

		private:
			Nz::RigidBody2D* GetRigidBody();

			void OnAttached() override;
			void OnComponentAttached(BaseComponent& component) override;
			void OnComponentDetached(BaseComponent& component) override;
			void OnDetached() override;
			void OnEntityDestruction() override;

			std::unique_ptr<Nz::RigidBody2D> m_object;
	};
}

#include <NDK/Components/PhysicsComponent2D.inl>

#endif // NDK_COMPONENTS_PHYSICSCOMPONENT2D_HPP
