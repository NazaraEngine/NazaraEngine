// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <type_traits>

namespace Ndk
{
	template<typename ComponentType>
	Component<ComponentType>::Component() :
	BaseComponent(GetComponentIndex<ComponentType>())
	{
	}

	template<typename ComponentType>
	Component<ComponentType>::~Component() = default;

	template<typename ComponentType>
	BaseComponent* Component<ComponentType>::Clone() const
	{
		///FIXME: Pas encore supporté par GCC (4.9.2)
		//static_assert(std::is_trivially_copy_constructible<ComponentType>::value, "ComponentType should be copy-constructible");

		return new ComponentType(static_cast<const ComponentType&>(*this));
	}
}
