// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandPool& owner, std::size_t poolIndex, std::size_t bindingIndex, Vk::AutoCommandBuffer commandBuffer) :
	m_bindingIndex(bindingIndex),
	m_poolIndex(poolIndex),
	m_commandBuffer(std::move(commandBuffer)),
	m_owner(owner)
	{
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

	inline const Vk::CommandBuffer& VulkanCommandBuffer::GetCommandBuffer() const
	{
		return m_commandBuffer;
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
