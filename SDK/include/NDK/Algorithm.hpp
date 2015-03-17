// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_ALGORITHM_HPP
#define NDK_ALGORITHM_HPP

#include <NDK/Prerequesites.hpp>

namespace Ndk
{
	template<unsigned int N> ComponentId BuildComponentId(const char (&id)[N]);
	template<unsigned int N> SystemId BuildSystemId(const char (&id)[N]);
	template<typename ComponentType> constexpr ComponentId GetComponentId();
	template<typename SystemType> constexpr SystemId GetSystemId();
}

#include <Ndk/Algorithm.inl>

#endif // NDK_ALGORITHM_HPP
