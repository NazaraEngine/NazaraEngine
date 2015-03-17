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

		nzUInt32 componentId = component->GetId();

		// Nous supprimons l'ancien component, s'il existe
		RemoveComponent(componentId);

		// Nous nous assurons que le vecteur de component est suffisamment grand pour contenir le nouveau component
		if (m_components.size() <= componentId)
			m_components.resize(componentId + 1);

		// Affectation et retour du component
		m_components[componentId] = std::move(component);
		m_componentBits.UnboundedSet(componentId);

		return *m_components[componentId].get();
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

	void Entity::RemoveAllComponents()
	{
		m_components.clear();
		m_componentBits.Clear();
	}

	void Entity::RemoveComponent(nzUInt32 componentId)
	{
		///DOC: N'a aucun effet si le component n'est pas présent
		if (HasComponent(componentId))
		{
			m_components[componentId].reset();
			m_componentBits.Reset(componentId);
		}
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
