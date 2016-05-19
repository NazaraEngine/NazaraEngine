// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkSurface.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Vulkan/VkInstance.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline CommandBuffer::CommandBuffer(CommandPool& pool, VkCommandBuffer handle) :
		m_pool(&pool),
		m_handle(handle)
		{
		}

		inline CommandBuffer::CommandBuffer(CommandBuffer&& commandBuffer) :
		m_pool(std::move(commandBuffer.m_pool)),
		m_allocator(commandBuffer.m_allocator),
		m_handle(commandBuffer.m_handle),
		m_lastErrorCode(commandBuffer.m_lastErrorCode)
		{
			commandBuffer.m_handle = VK_NULL_HANDLE;
		}

		inline CommandBuffer::~CommandBuffer()
		{
			Free();
		}

		inline void CommandBuffer::Free()
		{
			if (m_handle)
				m_pool->GetDevice().vkFreeCommandBuffers(m_pool->GetDevice(), *m_pool, 1, &m_handle);
		}

		inline VkResult CommandBuffer::GetLastErrorCode() const
		{
			return m_lastErrorCode;
		}

		inline CommandBuffer::operator VkCommandBuffer()
		{
			return m_handle;
		}
	}
}

#include <Nazara/Vulkan/DebugOff.hpp>
