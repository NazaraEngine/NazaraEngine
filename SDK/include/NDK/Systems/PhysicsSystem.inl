// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	/*!
	* \brief Gets the physical world
	* \return A reference to the physical world
	*/

	inline Nz::PhysWorld3D& PhysicsSystem::GetWorld()
	{
		return *m_world;
	}

	/*!
	* \brief Gets the physical world
	* \return A constant reference to the physical world
	*/

	inline const Nz::PhysWorld3D& PhysicsSystem::GetWorld() const
	{
		return *m_world;
	}
}
