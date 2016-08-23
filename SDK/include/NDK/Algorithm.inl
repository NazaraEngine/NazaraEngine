// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Endianness.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \brief Builds a component id based on a name
	* \return Identifier for the component
	*
	* \param name Name to generate id from
	*/

	///TODO: constexpr with the C++14
	template<unsigned int N>
	ComponentId BuildComponentId(const char (&name)[N])
	{
		static_assert(N-1 <= sizeof(ComponentId), "Name too long for this size of component id");

		ComponentId componentId = 0;
		for (unsigned int i = 0; i < N; ++i)
			componentId |= static_cast<ComponentId>(name[i]) << i*8;

		return componentId;
	}

	/*!
	* \ingroup NDK
	* \brief Gets the component id of a component
	* \return Identifier for the component
	*/

	template<typename ComponentType>
	ComponentIndex GetComponentIndex()
	{
		return ComponentType::componentIndex;
	}

	/*!
	* \ingroup NDK
	* \brief Gets the system id of a system
	* \return Identifier for the system
	*/

	template<typename SystemType>
	SystemIndex GetSystemIndex()
	{
		return SystemType::systemIndex;
	}

	/*!
	* \ingroup NDK
	* \brief Initializes the a component
	* \return Identifier for the component
	*
	* \param name Name to generate id from
	*/

	template<typename ComponentType, unsigned int N>
	ComponentIndex InitializeComponent(const char (&name)[N])
	{
		ComponentType::componentIndex = ComponentType::RegisterComponent(name);
		return ComponentType::componentIndex;
	}

	/*!
	* \ingroup NDK
	* \brief Initializes the a system
	* \return Identifier for the system
	*/

	template<typename SystemType>
	SystemIndex InitializeSystem()
	{
		SystemType::systemIndex = SystemType::RegisterSystem();
		return SystemType::systemIndex;
	}

	/*!
	* \brief Checks whether the parameter is a component
	* \return true If it is the case
	*
	* \param component Component to check
	*/

	template<typename ComponentType, typename C>
	bool IsComponent(C& component)
	{
		return component.GetIndex() == GetComponentIndex<ComponentType>();
	}

	/*!
	* \brief Checks whether the parameter is a system
	* \return true If it is the case
	*
	* \param system System to check
	*/

	template<typename SystemType, typename S>
	bool IsSystem(S& system)
	{
		return system.GetIndex() == GetSystemIndex<SystemType>();
	}
}
