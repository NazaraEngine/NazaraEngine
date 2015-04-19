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

	BaseComponent& Entity::AddComponent(std::unique_ptr<BaseComponent>&& componentPtr)
	{
		NazaraAssert(componentPtr, "Component must be valid");

		ComponentIndex index = componentPtr->GetIndex();

		// Nous nous assurons que le vecteur de component est suffisamment grand pour contenir le nouveau component
		if (index >= m_components.size())
			m_components.resize(index + 1);

		// Affectation et retour du component
		m_components[index] = std::move(componentPtr);
		m_componentBits.UnboundedSet(index);

		// On informe le monde que nous avons besoin d'une mise à jour
		m_world->Invalidate(m_id);

		// On récupère le component et on informe les composants existants du nouvel arrivant
		BaseComponent& component = *m_components[index].get();
		component.SetEntity(this);

		for (unsigned int i = m_componentBits.FindFirst(); i != m_componentBits.npos; i = m_componentBits.FindNext(i))
		{
			if (i != index)
				m_components[index]->OnComponentAttached(component);
		}

		return component;
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
		for (unsigned int i = m_componentBits.FindFirst(); i != m_componentBits.npos; i = m_componentBits.FindNext(i))
			RemoveComponent(i);

		NazaraAssert(m_componentBits.TestNone(), "All components should be gone");

		m_components.clear();

		// On informe le monde que nous avons besoin d'une mise à jour
		m_world->Invalidate(m_id);
	}

	void Entity::RemoveComponent(ComponentIndex index)
	{
		///DOC: N'a aucun effet si le component n'est pas présent
		if (HasComponent(index))
		{
			// On récupère le component et on informe les composants du détachement
			BaseComponent& component = *m_components[index].get();
			for (unsigned int i = m_componentBits.FindFirst(); i != m_componentBits.npos; i = m_componentBits.FindNext(i))
			{
				if (i != index)
					m_components[index]->OnComponentDetached(component);
			}

			component.SetEntity(nullptr);

			m_components[index].reset();
			m_componentBits.Reset(index);

			// On informe le monde que nous avons besoin d'une mise à jour
			m_world->Invalidate(m_id);
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
