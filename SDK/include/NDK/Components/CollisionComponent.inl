// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Entity.hpp>
#include <NDK/World.hpp>
#include <NDK/Components/PhysicsComponent.hpp>
#include <NDK/Systems/PhysicsSystem.hpp>

namespace Ndk
{
	/*!
	* \brief Constructs a CollisionComponent object with a geometry
	*
	* \param geom Reference to a geometry symbolizing the entity
	*/

	inline CollisionComponent::CollisionComponent(Nz::Collider3DRef geom) :
	m_geom(std::move(geom)),
	m_bodyUpdated(false)
	{
	}

	/*!
	* \brief Constructs a CollisionComponent object by copy semantic
	*
	* \param collision CollisionComponent to copy
	*/

	inline CollisionComponent::CollisionComponent(const CollisionComponent& collision) :
	m_geom(collision.m_geom),
	m_bodyUpdated(false)
	{
	}

	/*!
	* \brief Gets the geometry representing the entity
	* \return A constant reference to the physics geometry
	*/

	inline const Nz::Collider3DRef& CollisionComponent::GetGeom() const
	{
		return m_geom;
	}

	/*!
	* \brief Assigns the geometry to this component
	* \return A reference to this
	*
	* \param geom Reference to a geometry symbolizing the entity
	*/

	inline CollisionComponent& CollisionComponent::operator=(Nz::Collider3DRef geom)
	{
		SetGeom(geom);

		return *this;
	}

	/*!
	* \brief Gets the static body used by the entity
	* \return A pointer to the entity
	*/

	inline Nz::PhysObject* CollisionComponent::GetStaticBody()
	{
		return m_staticBody.get();
	}
}
