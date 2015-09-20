// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Debug.hpp>

template <typename T>
NzScopedXCB<T>::NzScopedXCB(T* pointer) :
m_pointer(pointer)
{
}

template <typename T>
NzScopedXCB<T>::~NzScopedXCB()
{
	std::free(m_pointer);
}

template <typename T>
T* NzScopedXCB<T>::operator ->() const
{
	return m_pointer;
}

template <typename T>
T** NzScopedXCB<T>::operator &()
{
	return &m_pointer;
}

template <typename T>
NzScopedXCB<T>::operator bool() const
{
	return m_pointer != nullptr;
}

template <typename T>
T* NzScopedXCB<T>::get() const
{
	return m_pointer;
}

#include <Nazara/Utility/DebugOff.hpp>
