// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

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

	BaseComponent& Entity::AddComponent(std::unique_ptr<BaseComponent>&& component)
	{
		NazaraAssert(component, "Component must be valid");

		ComponentIndex index = component->GetIndex();

		// Nous nous assurons que le vecteur de component est suffisamment grand pour contenir le nouveau component
		if (index >= m_components.size())
			m_components.resize(index + 1);

		// Affectation et retour du component
		m_components[index] = std::move(component);
		m_componentBits.UnboundedSet(index);

		// On informe le monde que nous avons besoin d'une mise à jour
		m_world->MarkAsDirty(m_id);

		return *m_components[index].get();
	}

	EntityHandle Entity::CreateHandle()
	{
		return EntityHandle(this);
	}

	void Entity::Kill()
	{
		m_world->KillEntity(this);
	}

	bool Entity::IsValid() const
	{
		return m_valid;
	}

	void Entity::RemoveAllComponents()
	{
		m_components.clear();
		m_componentBits.Clear();

		// On informe le monde que nous avons besoin d'une mise à jour
		m_world->MarkAsDirty(m_id);
	}

	void Entity::RemoveComponent(ComponentIndex index)
	{
		///DOC: N'a aucun effet si le component n'est pas présent
		if (HasComponent(index))
		{
			m_components[index].reset();
			m_componentBits.Reset(index);

			// On informe le monde que nous avons besoin d'une mise à jour
			m_world->MarkAsDirty(m_id);
		}
	}

	void Entity::Create()
	{
		m_valid = true;
	}

	void Entity::Destroy()
	{
		m_valid = false;

		// On informe chaque système
		for (SystemIndex index = m_systemBits.FindFirst(); index != m_systemBits.npos; index = m_systemBits.FindNext(index))
		{
			if (m_world->HasSystem(index))
			{
				BaseSystem& system = m_world->GetSystem(index);
				system.RemoveEntity(this);
			}
		}
		m_systemBits.Clear();

		// On informe chaque handle de notre destruction pour éviter qu'il ne continue de pointer sur nous
		for (EntityHandle* handle : m_handles)
			handle->OnEntityDestroyed();

		m_handles.clear();
	}
}
