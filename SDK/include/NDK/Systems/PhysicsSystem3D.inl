// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

namespace Ndk
{
	/*!
	* \brief Gets the physical world
	* \return A reference to the physical world
	*/

	inline Nz::PhysWorld3D& PhysicsSystem3D::GetWorld()
	{
		if (!m_world)
			CreatePhysWorld();

		return *m_world;
	}

	/*!
	* \brief Gets the physical world
	* \return A constant reference to the physical world
	*/

	inline const Nz::PhysWorld3D& PhysicsSystem3D::GetWorld() const
	{
		if (!m_world)
			CreatePhysWorld();

		return *m_world;
	}
}
