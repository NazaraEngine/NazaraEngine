// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	/*!
	* \brief Constructs a CollisionComponent2D object with a geometry
	*
	* \param geom Reference to a geometry symbolizing the entity
	*/

	inline CollisionComponent2D::CollisionComponent2D(Nz::Collider2DRef geom) :
	m_geom(std::move(geom)),
	m_bodyUpdated(false)
	{
	}

	/*!
	* \brief Constructs a CollisionComponent2D object by copy semantic
	*
	* \param collision CollisionComponent2D to copy
	*/

	inline CollisionComponent2D::CollisionComponent2D(const CollisionComponent2D& collision) :
	m_geom(collision.m_geom),
	m_bodyUpdated(false)
	{
	}

	/*!
	* \brief Gets the collision box representing the entity
	* \return The physics collision box
	*/

	inline Nz::Rectf CollisionComponent2D::GetAABB() const
	{
		return m_staticBody->GetAABB();
	}

	/*!
	* \brief Gets the geometry representing the entity
	* \return A constant reference to the physics geometry
	*/

	inline const Nz::Collider2DRef& CollisionComponent2D::GetGeom() const
	{
		return m_geom;
	}

	/*!
	* \brief Assigns the geometry to this component
	* \return A reference to this
	*
	* \param geom Reference to a geometry symbolizing the entity
	*/

	inline CollisionComponent2D& CollisionComponent2D::operator=(Nz::Collider2DRef geom)
	{
		SetGeom(geom);

		return *this;
	}

	/*!
	* \brief Gets the static body used by the entity
	* \return A pointer to the entity
	*/

	inline Nz::RigidBody2D& CollisionComponent2D::GetStaticBody()
	{
		return *m_staticBody.get();
	}
}
