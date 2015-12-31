// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Algorithm.hpp>
#include <type_traits>

namespace Ndk
{
	template<typename SystemType>
	System<SystemType>::System() :
	BaseSystem(GetSystemIndex<SystemType>())
	{
	}

	template<typename SystemType>
	System<SystemType>::~System() = default;

	template<typename SystemType>
	BaseSystem* System<SystemType>::Clone() const
	{
		///FIXME: Pas encore supporté par GCC (4.9.2)
		//static_assert(std::is_trivially_copy_constructible<SystemType>::value, "SystemType should be copy-constructible");

		return new SystemType(static_cast<const SystemType&>(*this));
	}

	template<typename SystemType>
	SystemIndex System<SystemType>::RegisterSystem()
	{
		return GetNextIndex();
	}
}
