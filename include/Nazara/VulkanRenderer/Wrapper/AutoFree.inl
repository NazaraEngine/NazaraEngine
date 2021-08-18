// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/AutoFree.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz::Vk
{
	template<typename T>
	template<typename... Args>
	AutoFree<T>::AutoFree(Args&&... args) :
	m_object(std::forward<Args>(args)...)
	{
	}

	template<typename T>
	AutoFree<T>::~AutoFree()
	{
		m_object.Free();
	}

	template<typename T>
	T& AutoFree<T>::Get()
	{
		return m_object;
	}

	template<typename T>
	const T& AutoFree<T>::Get() const
	{
		return m_object;
	}

	template<typename T>
	T* AutoFree<T>::operator->()
	{
		return &m_object;
	}

	template<typename T>
	const T* AutoFree<T>::operator->() const
	{
		return &m_object;
	}

	template<typename T>
	AutoFree<T>::operator T&()
	{
		return Get();
	}

	template<typename T>
	AutoFree<T>::operator const T&() const
	{
		return Get();
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
