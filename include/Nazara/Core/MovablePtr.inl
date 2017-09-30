// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/MovablePtr.hpp>
#include <cassert>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::MovablePtr
	* \brief Wraps a raw (non-proprietary) to allows it to be moved implicitly
	*/

	template<typename T>
	MovablePtr<T>::MovablePtr(T* value) :
	m_value(value)
	{
	}

	template<typename T>
	MovablePtr<T>::MovablePtr(MovablePtr&& ptr) noexcept :
	m_value(ptr.m_value)
	{
		ptr.m_value = nullptr;
	}

	template<typename T>
	inline T* MovablePtr<T>::Get() const
	{
		return m_value;
	}

	template<typename T>
	T* MovablePtr<T>::operator->() const
	{
		return m_value;
	}

	template<typename T>
	MovablePtr<T>::operator T*() const
	{
		return m_value;
	}

	template<typename T>
	inline MovablePtr<T>& MovablePtr<T>::operator=(T* value)
	{
		m_value = value;

		return *this;
	}

	template<typename T>
	MovablePtr<T>& MovablePtr<T>::operator=(MovablePtr&& ptr) noexcept
	{
		std::swap(m_value, ptr.m_value);
		return *this;
	}
}
