// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Entity.hpp>
#include <NDK/BaseComponent.hpp>
#include <NDK/World.hpp>

namespace Ndk
{
	Entity::Entity(Entity&& entity) :
	HandledObject(std::move(entity)),
	m_components(std::move(entity.m_components)),
	m_componentBits(std::move(entity.m_componentBits)),
	m_systemBits(std::move(entity.m_systemBits)),
	m_id(entity.m_id),
	m_world(entity.m_world),
	m_enabled(entity.m_enabled),
	m_valid(entity.m_valid)
	{
	}

	Entity::Entity(World* world, EntityId id) :
	m_id(id),
	m_world(world)
	{
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
		m_removedComponentBits.UnboundedReset(index);

		Invalidate();

		// On récupère le component et on informe les composants existants du nouvel arrivant
		BaseComponent& component = *m_components[index].get();
		component.SetEntity(this);

		for (std::size_t i = m_componentBits.FindFirst(); i != m_componentBits.npos; i = m_componentBits.FindNext(i))
		{
			if (i != index)
				m_components[i]->OnComponentAttached(component);
		}

		return component;
	}

	const EntityHandle& Entity::Clone() const
	{
		///DOC: The clone is enabled by default, even if the original entity is disabled
		NazaraAssert(IsValid(), "Invalid entity");

		return m_world->CloneEntity(m_id);
	}

	void Entity::Kill()
	{
		m_world->KillEntity(this);
	}

	void Entity::Invalidate()
	{
		// On informe le monde que nous avons besoin d'une mise à jour
		m_world->Invalidate(m_id);
	}

	void Entity::Create()
	{
		m_enabled = true;
		m_valid = true;
	}

	void Entity::Destroy()
	{
		// On informe chaque système
		for (std::size_t index = m_systemBits.FindFirst(); index != m_systemBits.npos; index = m_systemBits.FindNext(index))
		{
			if (m_world->HasSystem(index))
			{
				BaseSystem& system = m_world->GetSystem(index);
				system.RemoveEntity(this);
			}
		}
		m_systemBits.Clear();

		UnregisterAllHandles();

		m_valid = false;
	}

	void Entity::DestroyComponent(ComponentIndex index)
	{
		///DOC: N'a aucun effet si le component n'est pas présent
		if (HasComponent(index))
		{
			// On récupère le component et on informe les composants du détachement
			BaseComponent& component = *m_components[index].get();
			for (std::size_t i = m_componentBits.FindFirst(); i != m_componentBits.npos; i = m_componentBits.FindNext(i))
			{
				if (i != index)
					m_components[i]->OnComponentDetached(component);
			}

			component.SetEntity(nullptr);

			m_components[index].reset();
			m_componentBits.Reset(index);
		}
	}

}
