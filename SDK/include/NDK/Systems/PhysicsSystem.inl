// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	inline Nz::PhysWorld& PhysicsSystem::GetWorld()
	{
		return m_world;
	}

	inline const Nz::PhysWorld& PhysicsSystem::GetWorld() const
	{
		return m_world;
	}
}
