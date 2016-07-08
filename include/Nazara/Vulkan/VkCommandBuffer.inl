// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkCommandBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Vulkan/VkInstance.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline CommandBuffer::CommandBuffer() :
		m_pool(),
		m_handle(VK_NULL_HANDLE)
		{
		}

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
			m_lastErrorCode = m_pool->GetDevice()->vkBeginCommandBuffer(m_handle, &info);
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

		inline void CommandBuffer::BeginRenderPass(const VkRenderPassBeginInfo& beginInfo, VkSubpassContents contents)
		{
			return m_pool->GetDevice()->vkCmdBeginRenderPass(m_handle, &beginInfo, contents);
		}

		inline void CommandBuffer::BindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline)
		{
			return m_pool->GetDevice()->vkCmdBindPipeline(m_handle, pipelineBindPoint, pipeline);
		}

		inline void CommandBuffer::Draw(UInt32 vertexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance)
		{
			return m_pool->GetDevice()->vkCmdDraw(m_handle, vertexCount, instanceCount, firstVertex, firstInstance);
		}

		inline bool CommandBuffer::End()
		{
			m_lastErrorCode = m_pool->GetDevice()->vkEndCommandBuffer(m_handle);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to end command buffer");
				return false;
			}

			return true;
		}

		inline void CommandBuffer::EndRenderPass()
		{
			return m_pool->GetDevice()->vkCmdEndRenderPass(m_handle);
		}

		inline void CommandBuffer::Free()
		{
			if (m_handle)
				m_pool->GetDevice()->vkFreeCommandBuffers(*m_pool->GetDevice(), *m_pool, 1, &m_handle);
		}

		inline void CommandBuffer::PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, const VkImageMemoryBarrier& imageMemoryBarrier)
		{
			return PipelineBarrier(srcStageMask, dstStageMask, dependencyFlags, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
		}

		inline void CommandBuffer::PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, const VkMemoryBarrier& memoryBarrier, const VkBufferMemoryBarrier& bufferMemoryBarrier, const VkImageMemoryBarrier& imageMemoryBarrier)
		{
			return PipelineBarrier(srcStageMask, dstStageMask, dependencyFlags, 1, &memoryBarrier, 1, &bufferMemoryBarrier, 1, &imageMemoryBarrier);
		}

		inline void CommandBuffer::PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, UInt32 memoryBarrierCount, const VkMemoryBarrier* memoryBarriers, UInt32 bufferMemoryBarrierCount, const VkBufferMemoryBarrier* bufferMemoryBarriers, UInt32 imageMemoryBarrierCount, const VkImageMemoryBarrier* imageMemoryBarriers)
		{
			return m_pool->GetDevice()->vkCmdPipelineBarrier(m_handle, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, memoryBarriers, bufferMemoryBarrierCount, bufferMemoryBarriers, imageMemoryBarrierCount, imageMemoryBarriers);
		}

		inline void CommandBuffer::SetScissor(const Recti& scissorRegion)
		{
			VkRect2D rect = {
				{scissorRegion.x, scissorRegion.y},                         // VkOffset2D    offset
				{UInt32(scissorRegion.width), UInt32(scissorRegion.height)} // VkExtent2D    extent
			};

			SetScissor(rect);
		}

		inline void CommandBuffer::SetScissor(const VkRect2D& scissorRegion)
		{
			return SetScissor(0, 1, &scissorRegion);
		}

		inline void CommandBuffer::SetScissor(UInt32 firstScissor, UInt32 scissorCount, const VkRect2D* scissors)
		{
			return m_pool->GetDevice()->vkCmdSetScissor(m_handle, firstScissor, scissorCount, scissors);
		}

		inline void CommandBuffer::SetViewport(const Rectf& viewport, float minDepth, float maxDepth)
		{
			VkViewport rect = {
				viewport.x,      // float    x;
				viewport.y,      // float    y;
				viewport.width,  // float    width;
				viewport.height, // float    height;
				minDepth,        // float    minDepth;
				maxDepth         // float    maxDepth;
			};

			SetViewport(rect);
		}

		inline void CommandBuffer::SetViewport(const VkViewport& viewport)
		{
			return SetViewport(0, 1, &viewport);
		}

		inline void CommandBuffer::SetViewport(UInt32 firstViewport, UInt32 viewportCount, const VkViewport* viewports)
		{
			return m_pool->GetDevice()->vkCmdSetViewport(m_handle, firstViewport, viewportCount, viewports);
		}

		inline VkResult CommandBuffer::GetLastErrorCode() const
		{
			return m_lastErrorCode;
		}

		inline CommandBuffer& CommandBuffer::operator=(CommandBuffer&& commandBuffer)
		{
			m_allocator = commandBuffer.m_allocator;
			m_handle = commandBuffer.m_handle;
			m_lastErrorCode = commandBuffer.m_lastErrorCode;
			m_pool = std::move(commandBuffer.m_pool);
			m_handle = commandBuffer.m_handle;
			
			commandBuffer.m_handle = VK_NULL_HANDLE;

			return *this;
		}

		inline CommandBuffer::operator VkCommandBuffer() const
		{
			return m_handle;
		}
	}
}

#include <Nazara/Vulkan/DebugOff.hpp>
