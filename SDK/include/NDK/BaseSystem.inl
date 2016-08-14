// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/BaseSystem.hpp>
#include <Nazara/Core/Error.hpp>
#include <type_traits>

namespace Ndk
{
	inline BaseSystem::BaseSystem(SystemIndex systemId) :
	m_updateEnabled(true),
	m_systemIndex(systemId)
	{
		SetUpdateRate(30);
	}

	inline BaseSystem::BaseSystem(const BaseSystem& system) :
	m_excludedComponents(system.m_excludedComponents),
	m_requiredComponents(system.m_requiredComponents),
	m_systemIndex(system.m_systemIndex),
	m_updateEnabled(system.m_updateEnabled),
	m_updateCounter(0.f),
	m_updateRate(system.m_updateRate)
	{
	}

	inline void BaseSystem::Enable(bool enable)
	{
		m_updateEnabled = enable;
	}

	inline const std::vector<EntityHandle>& BaseSystem::GetEntities() const
	{
		return m_entities;
	}

	inline SystemIndex BaseSystem::GetIndex() const
	{
		return m_systemIndex;
	}

	inline float BaseSystem::GetUpdateRate() const
	{
		return (m_updateRate > 0.f) ? 1.f / m_updateRate : 0.f;
	}

	inline World& BaseSystem::GetWorld() const
	{
		return *m_world;
	}

	inline bool BaseSystem::IsEnabled() const
	{
		return m_updateEnabled;
	}

	inline bool BaseSystem::HasEntity(const Entity* entity) const
	{
		if (!entity)
			return false;

		return m_entityBits.UnboundedTest(entity->GetId());
	}

	inline void BaseSystem::SetUpdateRate(float updatePerSecond)
	{
		m_updateCounter = 0.f;
		m_updateRate = (updatePerSecond > 0.f) ? 1.f / updatePerSecond : 0.f; // 0.f means no limit
	}

	inline void BaseSystem::Update(float elapsedTime)
	{
		if (!IsEnabled())
			return;

		if (m_updateRate > 0.f)
		{
			m_updateCounter += elapsedTime;

			while (m_updateCounter >= m_updateRate)
			{
				OnUpdate(m_updateRate);
				m_updateCounter -= m_updateRate;
			}
		}
		else
			OnUpdate(elapsedTime);
	}

	template<typename ComponentType>
	void BaseSystem::Excludes()
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>::value , "ComponentType is not a component");

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
		static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "ComponentType is not a component");

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

	template<typename ComponentType>
	void BaseSystem::RequiresAny()
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "ComponentType is not a component");

		RequiresAnyComponent(GetComponentIndex<ComponentType>());
	}

	template<typename ComponentType1, typename ComponentType2, typename... Rest>
	void BaseSystem::RequiresAny()
	{
		RequiresAny<ComponentType1>();
		RequiresAny<ComponentType2, Rest...>();
	}

	inline void BaseSystem::RequiresAnyComponent(ComponentIndex index)
	{
		m_requiredAnyComponents.UnboundedSet(index);
	}

	inline void BaseSystem::AddEntity(Entity* entity)
	{
		NazaraAssert(entity, "Invalid entity");

		m_entities.emplace_back(entity);
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

	inline void BaseSystem::ValidateEntity(Entity* entity, bool justAdded)
	{
		NazaraAssert(entity, "Invalid entity");
		NazaraAssert(HasEntity(entity), "Entity should be part of system");

		OnEntityValidation(entity, justAdded);
	}

	inline void BaseSystem::SetWorld(World* world) noexcept
	{
		m_world = world;
	}

	inline bool BaseSystem::Initialize()
	{
		s_nextIndex = 0;

		return true;
	}

	inline void BaseSystem::Uninitialize()
	{
		// Nothing to do
	}
}
