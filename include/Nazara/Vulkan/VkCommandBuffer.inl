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

		inline bool CommandBuffer::Begin(const VkCommandBufferBeginInfo& info)
		{
			m_lastErrorCode = m_pool->GetDevice().vkBeginCommandBuffer(m_handle, &info);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to begin command buffer");
				return false;
			}

			return true;
		}

		inline bool CommandBuffer::Begin(VkCommandBufferUsageFlags flags)
		{
			VkCommandBufferBeginInfo beginInfo = {
				VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
				nullptr,
				flags,
				nullptr
			};

			return Begin(beginInfo);
		}

		inline bool CommandBuffer::Begin(VkCommandBufferUsageFlags flags, const VkCommandBufferInheritanceInfo& inheritanceInfo)
		{
			VkCommandBufferBeginInfo beginInfo = {
				VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
				nullptr,
				flags,
				&inheritanceInfo
			};

			return Begin(beginInfo);
		}

		inline bool CommandBuffer::Begin(VkCommandBufferUsageFlags flags, VkRenderPass renderPass, UInt32 subpass, VkFramebuffer framebuffer, bool occlusionQueryEnable, VkQueryControlFlags queryFlags, VkQueryPipelineStatisticFlags pipelineStatistics)
		{
			NazaraAssert(flags & VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, "Continue bit is required to ignore renderPass, subpass and framebuffer");

			VkCommandBufferInheritanceInfo inheritanceInfo = {
				VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
				nullptr,
				renderPass,
				subpass,
				framebuffer,
				VkBool32((occlusionQueryEnable) ? VK_TRUE : VK_FALSE),
				queryFlags,
				pipelineStatistics
			};

			VkCommandBufferBeginInfo beginInfo = {
				VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
				nullptr,
				flags,
				&inheritanceInfo
			};

			return Begin(beginInfo);
		}

		inline bool CommandBuffer::Begin(VkCommandBufferUsageFlags flags, bool occlusionQueryEnable, VkQueryControlFlags queryFlags, VkQueryPipelineStatisticFlags pipelineStatistics)
		{
			NazaraAssert(flags & VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, "Continue bit is required to ignore renderPass, subpass and framebuffer");

			VkCommandBufferInheritanceInfo inheritanceInfo = {
				VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
				nullptr,
				VK_NULL_HANDLE,
				0,
				VK_NULL_HANDLE,
				VkBool32((occlusionQueryEnable) ? VK_TRUE : VK_FALSE),
				queryFlags,
				pipelineStatistics
			};

			VkCommandBufferBeginInfo beginInfo = {
				VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
				nullptr,
				flags,
				&inheritanceInfo
			};

			return Begin(beginInfo);
		}

		inline bool CommandBuffer::End()
		{
			m_lastErrorCode = m_pool->GetDevice().vkEndCommandBuffer(m_handle);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to end command buffer");
				return false;
			}

			return true;
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
