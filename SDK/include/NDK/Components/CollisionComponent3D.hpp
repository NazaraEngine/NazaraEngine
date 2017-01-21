// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_COMPONENTS_COLLISIONCOMPONENT3D_HPP
#define NDK_COMPONENTS_COLLISIONCOMPONENT3D_HPP

#include <Nazara/Physics3D/Collider3D.hpp>
#include <NDK/Component.hpp>
#include <memory>

namespace Nz
{
	class RigidBody3D;
}

namespace Ndk
{
	class Entity;

	class NDK_API CollisionComponent3D : public Component<CollisionComponent3D>
	{
		friend class PhysicsSystem3D;

		public:
			CollisionComponent3D(Nz::Collider3DRef geom = Nz::Collider3DRef());
			CollisionComponent3D(const CollisionComponent3D& collision);
			~CollisionComponent3D() = default;

			const Nz::Collider3DRef& GetGeom() const;

			void SetGeom(Nz::Collider3DRef geom);

			CollisionComponent3D& operator=(Nz::Collider3DRef geom);
			CollisionComponent3D& operator=(CollisionComponent3D&& collision) = default;

			static ComponentIndex componentIndex;

		private:
			void InitializeStaticBody();
			Nz::RigidBody3D* GetStaticBody();

			void OnAttached() override;
			void OnComponentAttached(BaseComponent& component) override;
			void OnComponentDetached(BaseComponent& component) override;
			void OnDetached() override;

			std::unique_ptr<Nz::RigidBody3D> m_staticBody;
			Nz::Collider3DRef m_geom;
			bool m_bodyUpdated;
	};
}

#include <NDK/Components/CollisionComponent3D.inl>

#endif // NDK_COMPONENTS_COLLISIONCOMPONENT3D_HPP
