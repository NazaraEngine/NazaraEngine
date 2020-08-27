// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanCommandBuffer.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandPool& owner, std::size_t poolIndex, std::size_t bindingIndex, Vk::AutoCommandBuffer commandBuffer) :
	VulkanCommandBuffer(owner, poolIndex, bindingIndex)
	{
		m_commandBuffers.push_back(std::move(commandBuffer));
	}

	inline VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandPool& owner, std::size_t poolIndex, std::size_t bindingIndex, std::vector<Vk::AutoCommandBuffer> commandBuffers) :
	VulkanCommandBuffer(owner, poolIndex, bindingIndex)
	{
		m_commandBuffers = std::move(commandBuffers);
	}

	inline VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandPool& owner, std::size_t poolIndex, std::size_t bindingIndex) :
	m_bindingIndex(bindingIndex),
	m_poolIndex(poolIndex),
	m_owner(owner)
	{
	}

	inline std::size_t VulkanCommandBuffer::GetBindingIndex() const
	{
		return m_bindingIndex;
	}

	inline Vk::CommandBuffer& VulkanCommandBuffer::GetCommandBuffer(std::size_t imageIndex)
	{
		return m_commandBuffers[imageIndex].Get();
	}

	inline std::size_t VulkanCommandBuffer::GetPoolIndex() const
	{
		return m_poolIndex;
	}

	inline const VulkanCommandPool& VulkanCommandBuffer::GetOwner() const
	{
		return m_owner;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
