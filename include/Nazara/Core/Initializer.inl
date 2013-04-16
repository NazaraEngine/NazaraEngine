// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

template<typename...> struct NzImplInitializer;

template<typename T, typename... Rest>
struct NzImplInitializer<T, Rest...>
{
	static bool Init()
	{
		if (T::Initialize())
		{
			if (NzImplInitializer<Rest...>::Init())
				return true;
			else
				T::Uninitialize();
		}

		return false;
	}

	static void Uninit()
	{
		NzImplInitializer<Rest...>::Uninit();
		T::Uninitialize();
	}
};

template<>
struct NzImplInitializer<>
{
	static bool Init()
	{
		return true;
	}

	static void Uninit()
	{
	}
};

template<typename... Args>
NzInitializer<Args...>::NzInitializer()
{
	m_initialized = NzImplInitializer<Args...>::Init();
}

template<typename... Args>
NzInitializer<Args...>::~NzInitializer()
{
	if (m_initialized)
		NzImplInitializer<Args...>::Uninit();
}

template<typename... Args>
bool NzInitializer<Args...>::IsInitialized() const
{
	return m_initialized;
}

template<typename... Args>
NzInitializer<Args...>::operator bool() const
{
	return m_initialized;
}

#include <Nazara/Core/DebugOff.hpp>
