// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Initializer.hpp>
#include <Nazara/Core/Debug.hpp>

template<typename T>
template<typename... Args>
NzInitializer<T>::NzInitializer(Args... args)
{
	T::Initialize(args...);
}

template<typename T>
NzInitializer<T>::~NzInitializer()
{
	if (T::IsInitialized())
		T::Uninitialize();
}

template<typename T>
bool NzInitializer<T>::IsInitialized() const
{
	return T::IsInitialized();
}

template<typename T>
NzInitializer<T>::operator bool() const
{
	return T::IsInitialized();
}

#include <Nazara/Core/DebugOff.hpp>
