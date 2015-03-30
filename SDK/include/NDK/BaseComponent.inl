// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Ndk/Algorithm.hpp>
#include <type_traits>

namespace Ndk
{
	inline BaseComponent::BaseComponent(ComponentIndex index) :
	m_componentIndex(index)
	{
	}

	inline ComponentIndex BaseComponent::GetIndex() const
	{
		return m_componentIndex;
	}

	template<typename ComponentType, unsigned int N>
	ComponentIndex BaseComponent::Register(const char (&name)[N])
	{
		static_assert(std::is_default_constructible<ComponentType>::value, "ComponentType should be default-constructible");

		ComponentId id = BuildComponentId(name);
		auto factory = []() -> BaseComponent*
		{
			return new ComponentType;
		};

		return Register(id, factory);
	}

	inline ComponentIndex BaseComponent::Register(ComponentId id, Factory factoryFunc)
	{
		ComponentIndex index = s_entries.size();

		s_entries.resize(index + 1);
		ComponentEntry& entry = s_entries.back();
		entry.factory = factoryFunc;
		entry.id = id;

		s_idToIndex[id] = index;

		return index;
	}
}
