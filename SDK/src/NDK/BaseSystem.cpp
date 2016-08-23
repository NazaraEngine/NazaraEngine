// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/BaseSystem.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::BaseSystem
	* \brief NDK class that represents the common base of all systems
	*
	* \remark This class is meant to be purely abstract, for type erasure
	*/

	/*!
	* \brief Destructs the object and unregisters it-self on every entities
	*/

	BaseSystem::~BaseSystem()
	{
		for (const EntityHandle& entity : m_entities)
			entity->UnregisterSystem(m_systemIndex);
	}

	/*!
	* \brief Checks whether the key of the entity matches the lock of the system
	* \return true If it is the case
	*
	* \param Pointer to the entity
	*/

	bool BaseSystem::Filters(const Entity* entity) const
	{
		if (!entity)
			return false;

		const Nz::Bitset<>& components = entity->GetComponentBits();

		m_filterResult.PerformsAND(m_requiredComponents, components);
		if (m_filterResult !=  m_requiredComponents)
			return false; // At least one required component is not available

		m_filterResult.PerformsAND(m_excludedComponents, components);
		if (m_filterResult.TestAny())
			return false; // At least one excluded component is available

		// If we have a list of needed components
		if (m_requiredAnyComponents.TestAny())
		{
			if (!m_requiredAnyComponents.Intersects(components))
				return false;
		}

		return true;
	}

	/*!
	* \brief Operation to perform when entity is added to the system
	*
	* \param Pointer to the entity
	*/

	void BaseSystem::OnEntityAdded(Entity* entity)
	{
		NazaraUnused(entity);
	}

	/*!
	* \brief Operation to perform when entity is removed to the system
	*
	* \param Pointer to the entity
	*/

	void BaseSystem::OnEntityRemoved(Entity* entity)
	{
		NazaraUnused(entity);
	}

	/*!
	* \brief Operation to perform when entity is validated for the system
	*
	* \param entity Pointer to the entity
	* \param justAdded Is the entity newly added
	*/

	void BaseSystem::OnEntityValidation(Entity* entity, bool justAdded)
	{
		NazaraUnused(entity);
		NazaraUnused(justAdded);
	}

	SystemIndex BaseSystem::s_nextIndex;
}
