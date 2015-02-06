// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/Entity.hpp>
#include <NDK/World.hpp>

namespace Ndk
{
	Entity::Entity() :
	m_world(nullptr)
	{
		m_id.value = 0;
	}

	Entity::Entity(Id id, World* world) :
	m_id(id),
	m_world(world)
	{
	}

	void Entity::Kill()
	{
		m_world->KillEntity(*this);
	}

	Entity::Id Entity::GetId() const
	{
		return m_id;
	}

	World* Entity::GetWorld() const
	{
		return m_world;
	}

	bool Entity::IsValid() const
	{
		return m_world != nullptr && m_world->IsEntityIdValid(m_id);
	}

	bool Entity::operator==(const Entity& other) const
	{
		return m_world == other.m_world && m_id == other.m_id;
	}

	bool Entity::operator!=(const Entity& other) const
	{
		return !operator==(other);
	}
}
