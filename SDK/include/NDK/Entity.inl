// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/World.hpp>

namespace Ndk
{
	inline Entity::Entity() :
	m_world(nullptr)
	{
		m_id.value = 0;
	}

	inline Entity::Entity(Id id, World* world) :
	m_id(id),
	m_world(world)
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

	inline bool Entity::operator==(const Entity& other) const
	{
		return m_world == other.m_world && m_id == other.m_id;
	}

	inline bool Entity::operator!=(const Entity& other) const
	{
		return !operator==(other);
	}

	inline bool Entity::Id::operator==(const Id& other) const
	{
		return value == other.value;
	}

	inline bool Entity::Id::operator!=(const Id& other) const
	{
		return !operator==(other);
	}
}
