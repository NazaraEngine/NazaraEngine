// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <algorithm>
#include <Nazara/Core/Debug.hpp>

template<typename... Args>
void NzSignal<Args...>::Connect(const Callback& func)
{
	m_callbacks.push_back(func);
}

template<typename... Args>
void NzSignal<Args...>::operator()(Args&&... args)
{
	for (const Callback& func : m_callbacks)
		func(std::forward<Args>(args)...);
}

#include <Nazara/Core/DebugOff.hpp>
