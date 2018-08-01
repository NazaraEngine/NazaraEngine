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
			
			void AddForce(const Nz::Vector2f& force, Nz::CoordSys coordSys = Nz::CoordSys_Global);
			void AddForce(const Nz::Vector2f& force, const Nz::Vector2f& point, Nz::CoordSys coordSys = Nz::CoordSys_Global);
			void AddImpulse(const Nz::Vector2f& impulse, Nz::CoordSys coordSys = Nz::CoordSys_Global);
			void AddImpulse(const Nz::Vector2f& impulse, const Nz::Vector2f& point, Nz::CoordSys coordSys = Nz::CoordSys_Global);
			void AddTorque(float torque);

			bool ClosestPointQuery(const Nz::Vector2f& position, Nz::Vector2f* closestPoint, float* closestDistance) const;

			Nz::Rectf GetAABB() const;
			float GetAngularVelocity() const;
			Nz::Vector2f GetCenterOfGravity(Nz::CoordSys coordSys = Nz::CoordSys_Local) const;
			float GetMass() const;
			Nz::Vector2f GetPosition() const;
			float GetRotation() const;
			Nz::Vector2f GetVelocity() const;
			NAZARA_DEPRECATED("Name error, please use GetMassCenter");
			inline Nz::Vector2f GetCenterOfGravity(Nz::CoordSys coordSys = Nz::CoordSys_Local) const;
			inline Nz::Vector2f GetMassCenter(Nz::CoordSys coordSys = Nz::CoordSys_Local) const;

			bool IsSleeping() const;

			void SetAngularVelocity(float angularVelocity);
			void SetMass(float mass);
			void SetMassCenter(const Nz::Vector2f& center);
			void SetPosition(const Nz::Vector2f& position);
			void SetRotation(float rotation);
			void SetVelocity(const Nz::Vector2f& velocity);

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
