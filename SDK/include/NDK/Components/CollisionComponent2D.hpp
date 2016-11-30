// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_COMPONENTS_COLLISIONCOMPONENT2D_HPP
#define NDK_COMPONENTS_COLLISIONCOMPONENT2D_HPP

#include <Nazara/Physics2D/Collider2D.hpp>
#include <NDK/Component.hpp>
#include <memory>

namespace Nz
{
	class RigidBody2D;
}

namespace Ndk
{
	class Entity;

	class NDK_API CollisionComponent2D : public Component<CollisionComponent2D>
	{
		friend class PhysicsSystem2D;

		public:
			CollisionComponent2D(Nz::Collider2DRef geom = Nz::Collider2DRef());
			CollisionComponent2D(const CollisionComponent2D& collision);
			~CollisionComponent2D() = default;

			const Nz::Collider2DRef& GetGeom() const;

			void SetGeom(Nz::Collider2DRef geom);

			CollisionComponent2D& operator=(Nz::Collider2DRef geom);
			CollisionComponent2D& operator=(CollisionComponent2D&& collision) = default;

			static ComponentIndex componentIndex;

		private:
			void InitializeStaticBody();
			Nz::RigidBody2D* GetStaticBody();

			void OnAttached() override;
			void OnComponentAttached(BaseComponent& component) override;
			void OnComponentDetached(BaseComponent& component) override;
			void OnDetached() override;

			std::unique_ptr<Nz::RigidBody2D> m_staticBody;
			Nz::Collider2DRef m_geom;
			bool m_bodyUpdated;
	};
}

#include <NDK/Components/CollisionComponent2D.inl>

#endif // NDK_COMPONENTS_COLLISIONCOMPONENT2D_HPP
