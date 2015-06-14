// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Endianness.hpp>

namespace Ndk
{
	///TODO: constexpr avec le C++14
	template<unsigned int N>
	ComponentId BuildComponentId(const char (&name)[N])
	{
		static_assert(N-1 <= sizeof(ComponentId), "Name too long for this size of component id");

		ComponentId componentId = 0;
		for (unsigned int i = 0; i < N; ++i)
			componentId |= static_cast<ComponentId>(name[i]) << i*8;

		return componentId;
	}

	template<typename ComponentType>
	ComponentIndex GetComponentIndex()
	{
		return ComponentType::componentIndex;
	}

	template<typename SystemType>
	SystemIndex GetSystemIndex()
	{
		return SystemType::systemIndex;
	}

	template<typename ComponentType, unsigned int N>
	ComponentIndex InitializeComponent(const char (&name)[N])
	{
		ComponentType::componentIndex = ComponentType::RegisterComponent(name);
		return ComponentType::componentIndex;
	}

	template<typename SystemType>
	SystemIndex InitializeSystem()
	{
		SystemType::systemIndex = SystemType::RegisterSystem();
		return SystemType::systemIndex;
	}

	template<typename ComponentType, typename C>
	bool IsComponent(C& component)
	{
		return component.GetIndex() == GetComponentIndex<ComponentType>();
	}

	template<typename SystemType, typename S>
	bool IsSystem(S& system)
	{
		return system.GetIndex() == GetSystemIndex<SystemType>();
	}
}
