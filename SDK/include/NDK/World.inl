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

		SystemIndex index = system->GetIndex();

		// Nous nous assurons que le vecteur de component est suffisamment grand pour contenir le nouveau component
		if (index >= m_systems.size())
			m_systems.resize(index + 1);

		// Affectation et retour du système
		m_systems[index] = std::move(system);
		m_systems[index]->SetWorld(*this);

		MarkAllAsDirty(); // On force une mise à jour de toutes les entités

		return *m_systems[index].get();
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

	inline const World::EntityList& World::GetEntities()
	{
		return m_aliveEntities;
	}

	inline BaseSystem& World::GetSystem(SystemIndex index)
	{
		///DOC: Le système doit être présent
		NazaraAssert(HasSystem(index), "This system is not part of the world");

		BaseSystem* system = m_systems[index].get();
		NazaraAssert(system, "Invalid system pointer");

		return *system;
	}

	template<typename SystemType>
	SystemType& World::GetSystem()
	{
		///DOC: Le système doit être présent
		static_assert(std::is_base_of<BaseSystem, SystemType>(), "SystemType is not a system");

		SystemIndex index = GetSystemIndex<SystemType>();
		return static_cast<SystemType&>(GetSystem(index));
	}

	inline bool World::HasSystem(SystemIndex index) const
	{
		return index < m_systems.size() && m_systems[index];
	}

	template<typename SystemType>
	bool World::HasSystem() const
	{
		static_assert(std::is_base_of<BaseSystem, SystemType>(), "SystemType is not a component");

		SystemIndex index = GetSystemIndex<SystemType>();
		return HasSystem(index);
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

	inline void World::RemoveSystem(SystemIndex index)
	{
		///DOC: N'a aucun effet si le système n'est pas présent
		if (HasSystem(index))
			m_systems[index].reset();
	}

	template<typename SystemType>
	void World::RemoveSystem()
	{
		static_assert(std::is_base_of<BaseSystem, SystemType>(), "SystemType is not a system");

		SystemIndex index = GetSystemIndex<SystemType>();
		RemoveSystem(index);
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
