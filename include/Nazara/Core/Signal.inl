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
void NzSignal<Args...>::Connect(Callback&& func)
{
	m_callbacks.emplace_back(std::move(func));
}

template<typename... Args>
template<typename O>
void NzSignal<Args...>::Connect(O& object, void (O::*method) (Args...))
{
    return Connect([&object, method] (Args&&... args)
    {
        return (object .* method) (std::forward<Args>(args)...);
    });
}

template<typename... Args>
template<typename O>
void NzSignal<Args...>::Connect(O* object, void (O::*method)(Args...))
{
    return Connect([object, method] (Args&&... args)
    {
        return (object ->* method) (std::forward<Args>(args)...);
    });
}

template<typename... Args>
void NzSignal<Args...>::operator()(Args&&... args)
{
	for (const Callback& func : m_callbacks)
		func(std::forward<Args>(args)...);
}

#include <Nazara/Core/DebugOff.hpp>
