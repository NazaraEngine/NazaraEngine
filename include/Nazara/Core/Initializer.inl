// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

// http://www.easyrgb.com/index.php?X=MATH

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
