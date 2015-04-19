// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/BaseComponent.hpp>

namespace Ndk
{
	BaseComponent::~BaseComponent() = default;

	void BaseComponent::OnComponentAttached(BaseComponent& component)
	{
		NazaraUnused(component);
	}

	void BaseComponent::OnComponentDetached(BaseComponent& component)
	{
		NazaraUnused(component);
	}

	std::vector<BaseComponent::ComponentEntry> BaseComponent::s_entries;
	std::unordered_map<ComponentId, ComponentIndex> BaseComponent::s_idToIndex;
}
