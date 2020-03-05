// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

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

	inline Nz::RigidBody2D* CollisionComponent2D::GetStaticBody()
	{
		return m_staticBody.get();
	}

	inline const Nz::RigidBody2D* CollisionComponent2D::GetStaticBody() const
	{
		return m_staticBody.get();
	}
}
