// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <type_traits>

namespace Ndk
{
	template<typename ComponentType>
	Component<ComponentType>::Component() :
	BaseComponent(GetComponentId<ComponentType>())
	{
	}

	template<typename ComponentType>
	Component<ComponentType>::~Component() = default;

	template<typename ComponentType>
	virtual BaseComponent* Component<ComponentType>::Clone() const
	{
		static_assert<std::is_trivially_copy_constructible<ComponentType>::value, "ComponentType should be copy-constructible">

		return new ComponentType(static_cast<ComponentType&>(*this));
	}

	template<typename ComponentType>
	constexpr nzUInt32 GetComponentId()
	{
		return ComponentType::ComponentId;
	}
}
