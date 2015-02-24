// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/Entity.hpp>
#include <NDK/EntityHandle.hpp>
#include <NDK/World.hpp>

namespace Ndk
{
	Entity::Entity(Entity&& entity) :
	m_handles(std::move(entity.m_handles)),
	m_id(entity.m_id),
	m_world(entity.m_world),
	m_valid(entity.m_valid)
	{
		for (EntityHandle* handle : m_handles)
			handle->OnEntityMoved(this);
	}

	Entity::~Entity()
	{
		Destroy();
	}

	EntityHandle Entity::CreateHandle()
	{
		return EntityHandle(this);
	}

	void Entity::Kill()
	{
		m_world->KillEntity(CreateHandle());
	}

	bool Entity::IsValid() const
	{
		return m_valid;
	}

	void Entity::Create()
	{
		m_valid = true;
	}

	void Entity::Destroy()
	{
		m_valid = false;

		// On informe chaque handle de notre destruction pour éviter qu'il ne continue de pointer sur nous
		for (EntityHandle* handle : m_handles)
			handle->OnEntityDestroyed();

		m_handles.clear();
	}
}
