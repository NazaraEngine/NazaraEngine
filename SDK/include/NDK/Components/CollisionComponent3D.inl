// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	/*!
	* \brief Constructs a CollisionComponent3D object with a geometry
	*
	* \param geom Reference to a geometry symbolizing the entity
	*/

	inline CollisionComponent3D::CollisionComponent3D(Nz::Collider3DRef geom) :
	m_geom(std::move(geom)),
	m_bodyUpdated(false)
	{
	}

	/*!
	* \brief Constructs a CollisionComponent3D object by copy semantic
	*
	* \param collision CollisionComponent3D to copy
	*/

	inline CollisionComponent3D::CollisionComponent3D(const CollisionComponent3D& collision) :
	m_geom(collision.m_geom),
	m_bodyUpdated(false)
	{
	}

	/*!
	* \brief Gets the geometry representing the entity
	* \return A constant reference to the physics geometry
	*/

	inline const Nz::Collider3DRef& CollisionComponent3D::GetGeom() const
	{
		return m_geom;
	}

	/*!
	* \brief Assigns the geometry to this component
	* \return A reference to this
	*
	* \param geom Reference to a geometry symbolizing the entity
	*/

	inline CollisionComponent3D& CollisionComponent3D::operator=(Nz::Collider3DRef geom)
	{
		SetGeom(geom);

		return *this;
	}

	/*!
	* \brief Gets the static body used by the entity
	* \return A pointer to the entity
	*/

	inline Nz::RigidBody3D* CollisionComponent3D::GetStaticBody()
	{
		return m_staticBody.get();
	}
}
