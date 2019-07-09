// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_ALGORITHM_HPP
#define NDK_ALGORITHM_HPP

#include <NDK/Prerequisites.hpp>

namespace Ndk
{
	template<unsigned int N> ComponentId BuildComponentId(const char (&name)[N]);
	template<typename ComponentType> ComponentIndex GetComponentIndex();
	template<typename SystemType> SystemIndex GetSystemIndex();
	template<typename ComponentType, unsigned int N> ComponentIndex InitializeComponent(const char (&name)[N]);
	template<typename SystemType> SystemIndex InitializeSystem();
	template<typename ComponentType, typename C> bool IsComponent(C& component);
	template<typename SystemType, typename S> bool IsSystem(S& system);
}

#include <NDK/Algorithm.inl>

#endif // NDK_ALGORITHM_HPP
