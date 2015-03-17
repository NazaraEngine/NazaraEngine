// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Endianness.hpp>

namespace Ndk
{
	///TODO: constexpr avec le C++14
	template<unsigned int N>
	ComponentId BuildComponentId(const char (&id)[N])
	{
		static_assert(N-1 <= sizeof(ComponentId), "ID too long for this size of component id");

		ComponentId componentId = 0;
		for (int i = 0; i < N; ++i)
			componentId |= static_cast<ComponentId>(id[i]) << i*8;

		return componentId;
	}

	template<unsigned int N>
	SystemId BuildSystemId(const char (&id)[N])
	{
		static_assert(N-1 <= sizeof(ComponentId), "ID too long for this size of component id");

		ComponentId componentId = 0;
		for (int i = 0; i < N; ++i)
			componentId |= static_cast<ComponentId>(id[i]) << i*8;

		return componentId;
	}

	template<typename ComponentType>
	constexpr ComponentId GetComponentId()
	{
		return ComponentType::ComponentId;
	}

	template<typename SystemType>
	constexpr SystemId GetSystemId()
	{
		return SystemType::SystemId;
	}
}
