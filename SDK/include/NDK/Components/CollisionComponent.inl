// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Entity.hpp>
#include <NDK/World.hpp>
#include <NDK/Components/PhysicsComponent.hpp>
#include <NDK/Systems/PhysicsSystem.hpp>

namespace Ndk
{
	inline CollisionComponent::CollisionComponent(Nz::PhysGeomRef geom) :
	m_geom(std::move(geom)),
	m_bodyUpdated(false)
	{
	}

	inline CollisionComponent::CollisionComponent(const CollisionComponent& collision) :
	m_geom(collision.m_geom),
	m_bodyUpdated(false)
	{
	}

	inline const Nz::PhysGeomRef& CollisionComponent::GetGeom() const
	{
		return m_geom;
	}

	inline CollisionComponent& CollisionComponent::operator=(Nz::PhysGeomRef geom)
	{
		SetGeom(geom);

		return *this;
	}

	inline Nz::PhysObject* CollisionComponent::GetStaticBody()
	{
		return m_staticBody.get();
	}
}
