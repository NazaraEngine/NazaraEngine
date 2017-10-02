// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>

namespace Ndk
{
	/*!
	* \brief Constructs a BaseComponent object with an index
	*
	* \param index Index of the component
	*/

	inline BaseComponent::BaseComponent(ComponentIndex index) :
	m_componentIndex(index),
	m_entity(nullptr)
	{
	}

	/*!
	* \brief Gets the entity owning this component
	* \return A handle to the entity owning this component, may be invalid if no entity owns it.
	*/
	inline const EntityHandle& BaseComponent::GetEntity() const
	{
		return m_entity;
	}

	/*!
	* \brief Gets the index of the component
	* \return Index of the component
	*/
	inline ComponentIndex BaseComponent::GetIndex() const
	{
		return m_componentIndex;
	}

	/*!
	* \brief Gets the maximal index of the components
	* \return Index of the maximal component
	*/

	inline ComponentIndex BaseComponent::GetMaxComponentIndex()
	{
		return static_cast<ComponentIndex>(s_entries.size());
	}

	/*!
	* \brief Registers a component
	* \return Index of the registered component
	*
	* \param id Index of the component
	* \param factory Factory to create the component
	*
	* \remark Produces a NazaraAssert if the identifier is already in use
	*/

	inline ComponentIndex BaseComponent::RegisterComponent(ComponentId id, Factory factoryFunc)
	{
		// We add our component to the end
		ComponentIndex index = static_cast<ComponentIndex>(s_entries.size());
		s_entries.resize(index + 1);

		// We retrieve it and affect it
		ComponentEntry& entry = s_entries.back();
		entry.factory = factoryFunc;
		entry.id = id;

		// We ensure that id is not already in use
		NazaraAssert(s_idToIndex.find(id) == s_idToIndex.end(), "This id is already in use");

		s_idToIndex[id] = index;

		return index;
	}

	/*!
	* \brief Sets the entity on which the component operates
	*/

	inline void BaseComponent::SetEntity(Entity* entity)
	{
		if (m_entity != entity)
		{
			if (m_entity)
				OnDetached();

			m_entity = entity;
			if (m_entity)
				OnAttached();
		}
	}

	/*!
	* \brief Initializes the BaseComponent
	* \return true
	*/

	inline bool BaseComponent::Initialize()
	{
		// Nothing to do
		return true;
	}

	/*!
	* \brief Uninitializes the BaseComponent
	*/

	inline void BaseComponent::Uninitialize()
	{
		s_entries.clear();
		s_idToIndex.clear();
	}
}
