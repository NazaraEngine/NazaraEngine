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

		inline void CommandBuffer::BindDescriptorSet(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, UInt32 firstSet, const VkDescriptorSet& descriptorSets)
		{
			return BindDescriptorSets(pipelineBindPoint, layout, firstSet, 1U, &descriptorSets);
		}

		inline void CommandBuffer::BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, UInt32 firstSet, UInt32 descriptorSetCount, const VkDescriptorSet* descriptorSets)
		{
			return BindDescriptorSets(pipelineBindPoint, layout, firstSet, descriptorSetCount, descriptorSets, 0U, nullptr);
		}

		inline void CommandBuffer::BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, UInt32 firstSet, UInt32 descriptorSetCount, const VkDescriptorSet* descriptorSets, UInt32 dynamicOffsetCount, const UInt32* dynamicOffsets)
		{
			return m_pool->GetDevice()->vkCmdBindDescriptorSets(m_handle, pipelineBindPoint, layout, firstSet, descriptorSetCount, descriptorSets, dynamicOffsetCount, dynamicOffsets);
		}

		inline void CommandBuffer::BindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType)
		{
			return m_pool->GetDevice()->vkCmdBindIndexBuffer(m_handle, buffer, offset, indexType);
		}

		inline void CommandBuffer::BindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline)
		{
			return m_pool->GetDevice()->vkCmdBindPipeline(m_handle, pipelineBindPoint, pipeline);
		}

		inline void CommandBuffer::BindVertexBuffer(UInt32 binding, const VkBuffer buffer, const VkDeviceSize offset)
		{
			return BindVertexBuffers(binding, 1, &buffer, &offset);
		}

		inline void CommandBuffer::BindVertexBuffers(UInt32 firstBinding, UInt32 bindingCount, const VkBuffer* buffer, const VkDeviceSize* offset)
		{
			return m_pool->GetDevice()->vkCmdBindVertexBuffers(m_handle, firstBinding, bindingCount, buffer, offset);
		}

		inline void CommandBuffer::ClearAttachment(const VkClearAttachment& attachment, const VkClearRect& rect)
		{
			return ClearAttachments(1U, &attachment, 1U, &rect);
		}

		inline void CommandBuffer::ClearAttachments(UInt32 attachmentCount, const VkClearAttachment* attachments, UInt32 rectCount, const VkClearRect* rects)
		{
			return m_pool->GetDevice()->vkCmdClearAttachments(m_handle, attachmentCount, attachments, rectCount, rects);
		}

		inline void CommandBuffer::ClearColorImage(VkImage image, VkImageLayout imageLayout, const VkClearColorValue& color, const VkImageSubresourceRange& range)
		{
			return ClearColorImage(image, imageLayout, color, 1U, &range);
		}

		inline void CommandBuffer::ClearColorImage(VkImage image, VkImageLayout imageLayout, const VkClearColorValue& color, UInt32 rangeCount, const VkImageSubresourceRange* ranges)
		{
			return m_pool->GetDevice()->vkCmdClearColorImage(m_handle, image, imageLayout, &color, rangeCount, ranges);
		}

		inline void CommandBuffer::ClearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue& depthStencil, const VkImageSubresourceRange& range)
		{
			return ClearDepthStencilImage(image, imageLayout, depthStencil, 1U, &range);
		}

		inline void CommandBuffer::ClearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue& depthStencil, UInt32 rangeCount, const VkImageSubresourceRange * ranges)
		{
			return m_pool->GetDevice()->vkCmdClearDepthStencilImage(m_handle, image, imageLayout, &depthStencil, rangeCount, ranges);
		}

		inline void CommandBuffer::Draw(UInt32 vertexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance)
		{
			return m_pool->GetDevice()->vkCmdDraw(m_handle, vertexCount, instanceCount, firstVertex, firstInstance);
		}

		inline void CommandBuffer::DrawIndexed(UInt32 indexCount, UInt32 instanceCount, UInt32 firstVertex, Int32 vertexOffset, UInt32 firstInstance)
		{
			return m_pool->GetDevice()->vkCmdDrawIndexed(m_handle, indexCount, instanceCount, firstVertex, vertexOffset, firstInstance);
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

		inline void CommandBuffer::SetImageLayout(VkImage image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout)
		{
			return SetImageLayout(image, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, oldImageLayout, newImageLayout);
		}

		inline void CommandBuffer::SetImageLayout(VkImage image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, const VkImageSubresourceRange& subresourceRange)
		{
			return SetImageLayout(image, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, oldImageLayout, newImageLayout, subresourceRange);
		}

		inline void CommandBuffer::SetImageLayout(VkImage image, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout)
		{
			VkImageSubresourceRange imageRange = {
				VK_IMAGE_ASPECT_COLOR_BIT, // VkImageAspectFlags                     aspectMask
				0,                         // uint32_t                               baseMipLevel
				1,                         // uint32_t                               levelCount
				0,                         // uint32_t                               baseArrayLayer
				1                          // uint32_t                               layerCount
			};

			return SetImageLayout(image, srcStageMask, dstStageMask, oldImageLayout, newImageLayout, imageRange);
		}

		inline void CommandBuffer::SetImageLayout(VkImage image, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, const VkImageSubresourceRange& subresourceRange)
		{
			VkAccessFlags srcAccessMask;
			switch (oldImageLayout)
			{
				case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
					srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
					break;
				case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
					srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
					break;
				case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
					srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
					break;
				case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
					srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
					break;
				case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
					srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
					break;
				case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
					srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
					break;
				case VK_IMAGE_LAYOUT_PREINITIALIZED:
					srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
					break;
				case VK_IMAGE_LAYOUT_GENERAL:
				case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
				case VK_IMAGE_LAYOUT_UNDEFINED:
				default:
					srcAccessMask = 0;
					break;
			}

			VkAccessFlags dstAccessMask;
			switch (newImageLayout)
			{
				case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
					if (oldImageLayout != VK_IMAGE_LAYOUT_UNDEFINED)
						srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

					dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
					break;
				case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
					dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
					break;
				case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
					if (srcAccessMask == 0)
						srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;

					dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
					break;
				case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
					dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
					break;
				case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
					srcAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
					dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
					break;
				case VK_IMAGE_LAYOUT_GENERAL:
				case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
				case VK_IMAGE_LAYOUT_UNDEFINED:
				default:
					dstAccessMask = 0;
					break;
			}

			VkImageMemoryBarrier imageBarrier = {
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, // VkStructureType         sType
				nullptr,                                // const void*             pNext
				srcAccessMask,                          // VkAccessFlags           srcAccessMask
				dstAccessMask,                          // VkAccessFlags           dstAccessMask
				oldImageLayout,                         // VkImageLayout           oldLayout
				newImageLayout,                         // VkImageLayout           newLayout
				VK_QUEUE_FAMILY_IGNORED,                // uint32_t                srcQueueFamilyIndex
				VK_QUEUE_FAMILY_IGNORED,                // uint32_t                dstQueueFamilyIndex
				image,                                  // VkImage                 image
				subresourceRange                        // VkImageSubresourceRange subresourceRange
			};

			return PipelineBarrier(srcStageMask, dstStageMask, 0, imageBarrier);
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
