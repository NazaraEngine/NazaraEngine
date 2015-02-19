// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/World.hpp>

namespace Ndk
{
	inline Entity::Entity(World& world, Id id) :
	m_id(id),
	m_world(&world)
	{
	}

	inline Entity::Id Entity::GetId() const
	{
		return m_id;
	}

	inline World* Entity::GetWorld() const
	{
		return m_world;
	}

	inline void Entity::RegisterHandle(EntityHandle* handle)
	{
		m_handles.insert(handle);
	}

	inline void Entity::UnregisterHandle(EntityHandle* handle)
	{
		m_handles.erase(handle);
	}
}
