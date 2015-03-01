// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	template<typename ComponentType>
	Component<ComponentType>::Component() :
	BaseComponent(GetId())
	{
	}

	template<typename ComponentType>
	Component<ComponentType>::~Component() = default;

	template<typename ComponentType>
	nzUInt32 Component<ComponentType>::GetId()
	{
		return ComponentType::ComponentId;
	}
}
