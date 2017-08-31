// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Algorithm.hpp>
#include <type_traits>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::Component<ComponentType>
	* \brief NDK class that represents a component for an entity which interacts with a system
	*
	* \remark This class is meant to be derived as CRTP: "Component<Subtype>"
	*/

	/*!
	* \brief Constructs a Component object by default
	*/

	template<typename ComponentType>
	Component<ComponentType>::Component() :
	BaseComponent(GetComponentIndex<ComponentType>())
	{
	}

	template<typename ComponentType>
	Component<ComponentType>::~Component() = default;

	/*!
	* \brief Clones the component
	* \return The clone newly created
	*
	* \remark The component to clone should be trivially copy constructible
	*/

	template<typename ComponentType>
	std::unique_ptr<BaseComponent> Component<ComponentType>::Clone() const
	{
		///FIXME: Pas encore supporté par GCC (4.9.2)
		//static_assert(std::is_trivially_copy_constructible<ComponentType>::value, "ComponentType must be copy-constructible");

		return std::make_unique<ComponentType>(static_cast<const ComponentType&>(*this));
	}

	/*!
	* \brief Registers the component by assigning it an index
	*/

	template<typename ComponentType>
	ComponentIndex Component<ComponentType>::RegisterComponent(ComponentId id)
	{
		//We use the lambda to create a factory function
		auto factory = []() -> BaseComponent*
		{
			return nullptr; //< Temporary workaround to allow non-default-constructed components, will be updated for serialization
			//return new ComponentType;
		};

		return BaseComponent::RegisterComponent(id, factory);
	}

	/*!
	* \brief Registers the component by assigning it an index based on the name
	*/

	template<typename ComponentType>
	template<unsigned int N>
	ComponentIndex Component<ComponentType>::RegisterComponent(const char (&name)[N])
	{
		// We convert the string to a number which will be used as unique identifier
		ComponentId id = BuildComponentId(name);
		return RegisterComponent(id);
	}
}
