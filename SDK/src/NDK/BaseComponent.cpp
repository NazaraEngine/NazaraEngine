// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/BaseComponent.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::BaseComponent
	* \brief NDK class that represents the common base of all components
	*
	* \remark This class is meant to be purely abstract, for type erasure
	*/

	BaseComponent::~BaseComponent() = default;

	/*!
	* \brief Operation to perform when component is attached to an entity
	*/

	void BaseComponent::OnAttached()
	{
	}

	/*!
	* \brief Operation to perform when component is attached to this component
	*
	* \param component Component being attached
	*/

	void BaseComponent::OnComponentAttached(BaseComponent& component)
	{
		NazaraUnused(component);
	}

	/*!
	* \brief Operation to perform when component is detached from this component
	*
	* \param component Component being detached
	*/

	void BaseComponent::OnComponentDetached(BaseComponent& component)
	{
		NazaraUnused(component);
	}

	/*!
	* \brief Operation to perform when component is detached from an entity
	*/

	void BaseComponent::OnDetached()
	{
	}

	/*!
	* \brief Operation to perform when the entity is destroyed and we're still attached to it
	*
	* \remark This is always called before the entity proper destruction, and thus its components.
	*/
	void BaseComponent::OnEntityDestruction()
	{
	}

	/*!
	* \brief Operation to perform when the entity is disabled
	*
	* \remark Disabling an entity will remove it from systems it belongs to, but sometimes the entity will need to do
	*         additional work in order to be properly disabled (i.e.: disabling physics simulation & collisions)
	*/
	void BaseComponent::OnEntityDisabled()
	{
	}

	/*!
	* \brief Operation to perform when the entity is disabled
	*
	* \remark Enabling an entity will add it back to systems it belongs to, but sometimes the entity will need to do
	*         additional work in order to be properly re-enabled (i.e.: enabling physics simulation & collisions)
	*/
	void BaseComponent::OnEntityEnabled()
	{
	}

	std::vector<BaseComponent::ComponentEntry> BaseComponent::s_entries;
	std::unordered_map<ComponentId, ComponentIndex> BaseComponent::s_idToIndex;
}
