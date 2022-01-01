// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanBuffer.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanBuffer::VulkanBuffer(Vk::Device& device, BufferType type) :
	m_type(type),
	m_device(device)
	{
	}

	inline VkBuffer VulkanBuffer::GetBuffer() const
	{
		return m_buffer;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
