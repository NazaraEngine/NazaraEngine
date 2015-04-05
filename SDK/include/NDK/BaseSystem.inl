// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>
#include <type_traits>

namespace Ndk
{
	inline BaseSystem::BaseSystem(SystemIndex systemId) :
	m_systemIndex(systemId)
	{
	}

	inline const std::vector<EntityHandle>& BaseSystem::GetEntities() const
	{
		return m_entities;
	}

	inline SystemIndex BaseSystem::GetIndex() const
	{
		return m_systemIndex;
	}

	inline World& BaseSystem::GetWorld() const
	{
		return *m_world;
	}

	inline bool BaseSystem::HasEntity(const Entity* entity) const
	{
		if (!entity)
			return false;

		return m_entityBits.UnboundedTest(entity->GetId());
	}

	template<typename ComponentType>
	void BaseSystem::Excludes()
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>(), "ComponentType is not a component");

		ExcludesComponent(GetComponentIndex<ComponentType>());
	}

	template<typename ComponentType1, typename ComponentType2, typename... Rest>
	void BaseSystem::Excludes()
	{
		Excludes<ComponentType1>();
		Excludes<ComponentType2, Rest...>();
	}

	inline void BaseSystem::ExcludesComponent(ComponentIndex index)
	{
		m_excludedComponents.UnboundedSet(index);
	}

	inline SystemIndex BaseSystem::GetNextIndex()
	{
		return s_nextIndex++;
	}

	template<typename ComponentType>
	void BaseSystem::Requires()
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>(), "ComponentType is not a component");

		RequiresComponent(GetComponentIndex<ComponentType>());
	}

	template<typename ComponentType1, typename ComponentType2, typename... Rest>
	void BaseSystem::Requires()
	{
		Requires<ComponentType1>();
		Requires<ComponentType2, Rest...>();
	}

	inline void BaseSystem::RequiresComponent(ComponentIndex index)
	{
		m_requiredComponents.UnboundedSet(index);
	}

	inline void BaseSystem::AddEntity(Entity* entity)
	{
		NazaraAssert(entity, "Invalid entity");

		m_entities.push_back(entity->CreateHandle());
		m_entityBits.UnboundedSet(entity->GetId(), true);

		entity->RegisterSystem(m_systemIndex);

		OnEntityAdded(entity);
	}

	inline void BaseSystem::RemoveEntity(Entity* entity)
	{
		NazaraAssert(entity, "Invalid entity");

		auto it = std::find(m_entities.begin(), m_entities.end(), *entity);
		NazaraAssert(it != m_entities.end(), "Entity is not part of this system");

		// Pour éviter de déplacer beaucoup de handles, on swap le dernier avec celui à supprimer
		std::swap(*it, m_entities.back());
		m_entities.pop_back(); // On le sort du vector

		m_entityBits.Reset(entity->GetId());
		entity->UnregisterSystem(m_systemIndex);

		OnEntityRemoved(entity); // Et on appelle le callback
	}

	inline void BaseSystem::SetWorld(World& world)
	{
		m_world = &world;
	}
}
