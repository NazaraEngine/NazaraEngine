// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_COMPONENTS_COLLISIONCOMPONENT_HPP
#define NDK_COMPONENTS_COLLISIONCOMPONENT_HPP

#include <Nazara/Physics3D/Geom.hpp>
#include <NDK/Component.hpp>
#include <memory>

namespace Nz
{
	class PhysObject;
}

namespace Ndk
{
	class Entity;

	class NDK_API CollisionComponent : public Component<CollisionComponent>
	{
		friend class PhysicsSystem;
		friend class StaticCollisionSystem;

		public:
			CollisionComponent(Nz::PhysGeomRef geom = Nz::PhysGeomRef());
			CollisionComponent(const CollisionComponent& collision);
			~CollisionComponent() = default;

			const Nz::PhysGeomRef& GetGeom() const;

			void SetGeom(Nz::PhysGeomRef geom);

			CollisionComponent& operator=(Nz::PhysGeomRef geom);
			CollisionComponent& operator=(CollisionComponent&& collision) = default;

			static ComponentIndex componentIndex;

		private:
			void InitializeStaticBody();
			Nz::PhysObject* GetStaticBody();

			void OnAttached() override;
			void OnComponentAttached(BaseComponent& component) override;
			void OnComponentDetached(BaseComponent& component) override;
			void OnDetached() override;

			std::unique_ptr<Nz::PhysObject> m_staticBody;
			Nz::PhysGeomRef m_geom;
			bool m_bodyUpdated;
	};
}

#include <NDK/Components/CollisionComponent.inl>

#endif // NDK_COMPONENTS_COLLISIONCOMPONENT_HPP
