// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/Entity.hpp>
#include <NDK/World.hpp>

namespace Ndk
{
	void Entity::Kill()
	{
		m_world->KillEntity(*this);
	}

	bool Entity::IsValid() const
	{
		return m_world != nullptr && m_world->IsEntityIdValid(m_id);
	}
}
