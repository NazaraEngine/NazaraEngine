// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_COMPONENTS_COLLISIONCOMPONENT_HPP
#define NDK_COMPONENTS_COLLISIONCOMPONENT_HPP

#include <Nazara/Physics/Geom.hpp>
#include <NDK/Component.hpp>
#include <memory>

class NzPhysObject;

namespace Ndk
{
	class Entity;

	class NDK_API CollisionComponent : public Component<CollisionComponent>
	{
		friend class PhysicsSystem;
		friend class StaticCollisionSystem;

		public:
			CollisionComponent(NzPhysGeomRef geom = NzPhysGeomRef());
			CollisionComponent(const CollisionComponent& collision);
			~CollisionComponent() = default;

			const NzPhysGeomRef& GetGeom() const;

			void SetGeom(NzPhysGeomRef geom);

			CollisionComponent& operator=(NzPhysGeomRef geom);
			CollisionComponent& operator=(CollisionComponent&& collision) = default;

			static ComponentIndex componentIndex;

		private:
			void InitializeStaticBody();
			NzPhysObject* GetStaticBody();

			void OnAttached() override;
			void OnComponentAttached(BaseComponent& component) override;
			void OnComponentDetached(BaseComponent& component) override;
			void OnDetached() override;

			std::unique_ptr<NzPhysObject> m_staticBody;
			NzPhysGeomRef m_geom;
			bool m_bodyUpdated;
	};
}

#include <NDK/Components/CollisionComponent.inl>

#endif // NDK_COMPONENTS_COLLISIONCOMPONENT_HPP
