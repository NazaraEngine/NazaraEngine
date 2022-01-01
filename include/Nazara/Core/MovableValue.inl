// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/MovableValue.hpp>
#include <utility>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	template<typename T>
	MovableValue<T>::MovableValue(T value) :
	m_value(std::move(value))
	{
	}

	template<typename T>
	MovableValue<T>::MovableValue(MovableValue&& val) noexcept :
	m_value()
	{
		std::swap(m_value, val.m_value);
	}

	template<typename T>
	T& MovableValue<T>::Get()
	{
		return m_value;
	}

	template<typename T>
	const T& MovableValue<T>::Get() const
	{
		return m_value;
	}

	template<typename T>
	MovableValue<T>::operator T&()
	{
		return m_value;
	}

	template<typename T>
	MovableValue<T>::operator const T&() const
	{
		return m_value;
	}

	template<typename T>
	MovableValue<T>& MovableValue<T>::operator=(T value)
	{
		m_value = std::move(value);

		return *this;
	}

	template<typename T>
	MovableValue<T>& MovableValue<T>::operator=(MovableValue&& ptr) noexcept
	{
		std::swap(m_value, ptr.m_value);
		return *this;
	}
}

#include <Nazara/Core/DebugOff.hpp>
