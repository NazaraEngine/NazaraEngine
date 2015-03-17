// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>
#include <type_traits>

namespace Ndk
{
	inline BaseSystem& World::AddSystem(std::unique_ptr<BaseSystem>&& system)
	{
		NazaraAssert(system, "System must be valid");

		SystemId systemId = system->GetId();

		// Affectation et retour du système
		m_systems[systemId] = std::move(system);
		m_systems[systemId]->SetWorld(*this);

		MarkAllAsDirty(); // On force une mise à jour de toutes les entités

		return *m_systems[systemId].get();
	}

	template<typename SystemType, typename... Args>
	SystemType& World::AddSystem(Args&&... args)
	{
		static_assert(std::is_base_of<BaseSystem, SystemType>(), "SystemType is not a component");

		// Allocation et affectation du component
		std::unique_ptr<SystemType> ptr(new SystemType(std::forward(args)...));
		return static_cast<SystemType&>(AddSystem(std::move(ptr)));
	}

	inline World::EntityList World::CreateEntities(unsigned int count)
	{
		EntityList list;
		list.reserve(count);

		for (unsigned int i = 0; i < count; ++i)
			list.emplace_back(CreateEntity());

		return list;
	}

	inline BaseSystem& World::GetSystem(SystemId systemId)
	{
		///DOC: Le système doit être présent
		NazaraAssert(HasSystem(systemId), "This system is not part of the world");

		BaseSystem* system = m_systems[systemId].get();
		NazaraAssert(system, "Invalid system pointer");

		return *system;
	}

	template<typename SystemType>
	SystemType& World::GetSystem()
	{
		///DOC: Le système doit être présent
		static_assert(std::is_base_of<BaseSystem, SystemType>(), "SystemType is not a system");

		SystemId systemId = GetSystemId<SystemType>();
		return static_cast<SystemType&>(GetSystem(systemId));
	}

	inline bool World::HasSystem(SystemId systemId) const
	{
		return m_systems.count(systemId) > 0;
	}

	template<typename SystemType>
	bool World::HasSystem() const
	{
		static_assert(std::is_base_of<BaseSystem, SystemType>(), "SystemType is not a component");

		SystemId systemId = GetSystemId<SystemType>();
		return HasSystem(systemId);
	}

	inline void World::KillEntities(const EntityList& list)
	{
		for (const EntityHandle& entity : list)
			KillEntity(entity);
	}

	inline bool World::IsEntityValid(const Entity* entity) const
	{
		return entity && entity->GetWorld() == this && IsEntityIdValid(entity->GetId());
	}

	inline bool World::IsEntityIdValid(EntityId id) const
	{
		return id < m_entities.size() && m_entities[id].entity.IsValid();
	}

	inline void World::RemoveAllSystems()
	{
		m_systems.clear();
	}

	inline void World::RemoveSystem(SystemId systemId)
	{
		///DOC: N'a aucun effet si le système n'est pas présent
		if (HasSystem(systemId))
			m_systems[systemId].reset();
	}

	template<typename SystemType>
	void World::RemoveSystem()
	{
		static_assert(std::is_base_of<BaseSystem, SystemType>(), "SystemType is not a system");

		SystemId systemId = GetSystemId<SystemType>();
		RemoveSystem(systemId);
	}

	inline void World::MarkAllAsDirty()
	{
		m_dirtyEntities.Resize(m_entities.size(), false);
		m_dirtyEntities.Set(true); // Activation de tous les bits
	}

	inline void World::MarkAsDirty(EntityId id)
	{
		m_dirtyEntities.UnboundedSet(id, true);
	}
}
