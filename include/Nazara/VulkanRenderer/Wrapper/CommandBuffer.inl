// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Instance.hpp>
#include <cassert>

namespace Nz::Vk
{
	inline CommandBuffer::CommandBuffer() :
	m_pool(nullptr),
	m_handle(VK_NULL_HANDLE)
	{
	}

	inline CommandBuffer::CommandBuffer(CommandPool& pool, VkCommandBuffer handle) :
	m_pool(&pool),
	m_handle(handle)
	{
	}

	inline CommandBuffer::CommandBuffer(CommandBuffer&& commandBuffer) noexcept :
	m_pool(commandBuffer.m_pool),
	m_handle(commandBuffer.m_handle),
	m_lastErrorCode(commandBuffer.m_lastErrorCode)
	{
		commandBuffer.m_handle = VK_NULL_HANDLE;
	}

	inline bool CommandBuffer::Begin(const VkCommandBufferBeginInfo& info)
	{
		m_lastErrorCode = m_pool->GetDevice()->vkBeginCommandBuffer(m_handle, &info);
		if (m_lastErrorCode != VkResult::VK_SUCCESS)
		{
			NazaraErrorFmt("failed to begin command buffer: {0}", TranslateVulkanError(m_lastErrorCode));
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

	inline void CommandBuffer::BeginDebugRegion(const char* label)
	{
		return BeginDebugRegion(label, Color::Black());
	}

	inline void CommandBuffer::BeginDebugRegion(const char* label, Color color)
	{
		Vk::Device* device = m_pool->GetDevice();
		if (device->vkCmdBeginDebugUtilsLabelEXT)
		{
			VkDebugUtilsLabelEXT debugLabel = {
				VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
				nullptr,
				label,
				{
					color.r,
					color.g,
					color.b,
					color.a
				}
			};

			device->vkCmdBeginDebugUtilsLabelEXT(m_handle, &debugLabel);
		}
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

	inline void CommandBuffer::BlitImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, const VkImageBlit& region, VkFilter filter)
	{
		return BlitImage(srcImage, srcImageLayout, dstImage, dstImageLayout, 1U, &region, filter);
	}

	inline void CommandBuffer::BlitImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, UInt32 regionCount, const VkImageBlit* regions, VkFilter filter)
	{
		return m_pool->GetDevice()->vkCmdBlitImage(m_handle, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, regions, filter);
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

	inline void CommandBuffer::CopyBuffer(VkBuffer source, VkBuffer target, UInt64 size, UInt64 sourceOffset, UInt64 targetOffset)
	{
		VkBufferCopy region;
		region.dstOffset = targetOffset;
		region.size = size;
		region.srcOffset = sourceOffset;

		return m_pool->GetDevice()->vkCmdCopyBuffer(m_handle, source, target, 1, &region);
	}

	inline void CommandBuffer::CopyBufferToImage(VkBuffer source, VkImage target, VkImageLayout targetLayout, UInt32 width, UInt32 height, UInt32 depth)
	{
		VkImageSubresourceLayers subresourceLayers = {
			VK_IMAGE_ASPECT_COLOR_BIT, //< aspectMask
			0,
			0,
			1
		};

		return CopyBufferToImage(source, target, targetLayout, subresourceLayers, width, height, depth);
	}

	inline void CommandBuffer::CopyBufferToImage(VkBuffer source, VkImage target, VkImageLayout targetLayout, const VkImageSubresourceLayers& subresourceLayers, Int32 x, Int32 y, Int32 z, UInt32 width, UInt32 height, UInt32 depth)
	{
		VkBufferImageCopy region = {
			0,
			0,
			0,
			subresourceLayers,
			{ // imageOffset
				x, y, z
			},
			{ // imageExtent
				width, height, depth
			}
		};

		return CopyBufferToImage(source, target, targetLayout, region);
	}

	inline void CommandBuffer::CopyBufferToImage(VkBuffer source, VkImage target, VkImageLayout targetLayout, const VkImageSubresourceLayers& subresourceLayers, UInt32 width, UInt32 height, UInt32 depth)
	{
		VkBufferImageCopy region = {
			0,
			0,
			0,
			subresourceLayers,
			{ // imageOffset
				0, 0, 0
			},
			{ // imageExtent
				width, height, depth
			}
		};

		return CopyBufferToImage(source, target, targetLayout, region);
	}

	inline void CommandBuffer::CopyBufferToImage(VkBuffer source, VkImage target, VkImageLayout targetLayout, const VkBufferImageCopy& region)
	{
		return CopyBufferToImage(source, target, targetLayout, 1, &region);
	}

	inline void CommandBuffer::CopyBufferToImage(VkBuffer source, VkImage target, VkImageLayout targetLayout, UInt32 regionCount, const VkBufferImageCopy* regions)
	{
		return m_pool->GetDevice()->vkCmdCopyBufferToImage(m_handle, source, target, targetLayout, regionCount, regions);
	}

	inline void CommandBuffer::CopyImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, const VkImageCopy& region)
	{
		return CopyImage(srcImage, srcImageLayout, dstImage, dstImageLayout, 1U, &region);
	}

	inline void CommandBuffer::CopyImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, UInt32 regionCount, const VkImageCopy* regions)
	{
		return m_pool->GetDevice()->vkCmdCopyImage(m_handle, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, regions);
	}

	inline void CommandBuffer::Dispatch(UInt32 groupCountX, UInt32 groupCountY, UInt32 groupCountZ)
	{
		return m_pool->GetDevice()->vkCmdDispatch(m_handle, groupCountX, groupCountY, groupCountZ);
	}

	inline void CommandBuffer::Draw(UInt32 vertexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance)
	{
		return m_pool->GetDevice()->vkCmdDraw(m_handle, vertexCount, instanceCount, firstVertex, firstInstance);
	}

	inline void CommandBuffer::DrawIndexed(UInt32 indexCount, UInt32 instanceCount, UInt32 firstIndex, Int32 vertexOffset, UInt32 firstInstance)
	{
		return m_pool->GetDevice()->vkCmdDrawIndexed(m_handle, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

	inline bool CommandBuffer::End()
	{
		m_lastErrorCode = m_pool->GetDevice()->vkEndCommandBuffer(m_handle);
		if (m_lastErrorCode != VkResult::VK_SUCCESS)
		{
			NazaraErrorFmt("failed to end command buffer: {0}", TranslateVulkanError(m_lastErrorCode));
			return false;
		}

		return true;
	}

	inline void CommandBuffer::EndDebugRegion()
	{
		Vk::Device* device = m_pool->GetDevice();
		if (device->vkCmdEndDebugUtilsLabelEXT)
			device->vkCmdEndDebugUtilsLabelEXT(m_handle);
	}

	inline void CommandBuffer::EndRenderPass()
	{
		return m_pool->GetDevice()->vkCmdEndRenderPass(m_handle);
	}

	inline void CommandBuffer::Free()
	{
		if (m_handle)
		{
			assert(m_pool);
			m_pool->GetDevice()->vkFreeCommandBuffers(*m_pool->GetDevice(), *m_pool, 1, &m_handle);
		}
	}

	inline VkResult CommandBuffer::GetLastErrorCode() const
	{
		return m_lastErrorCode;
	}

	inline CommandPool& CommandBuffer::GetPool()
	{
		return *m_pool;
	}

	inline void CommandBuffer::ImageBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout, VkImage image, const VkImageSubresourceRange& subresourceRange)
	{
		VkImageMemoryBarrier imageBarrier = {
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			nullptr,
			srcAccessMask,
			dstAccessMask,
			oldLayout,
			newLayout,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED,
			image,
			subresourceRange
		};

		return PipelineBarrier(srcStageMask, dstStageMask, dependencyFlags, imageBarrier);
	}

	inline void CommandBuffer::InsertDebugLabel(const char* label)
	{
		return InsertDebugLabel(label, Color::Black());
	}

	inline void CommandBuffer::InsertDebugLabel(const char* label, Color color)
	{
		Vk::Device* device = m_pool->GetDevice();
		if (device->vkCmdInsertDebugUtilsLabelEXT)
		{
			VkDebugUtilsLabelEXT debugLabel = {
				VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
				nullptr,
				label,
				{
					color.r,
					color.g,
					color.b,
					color.a
				}
			};

			device->vkCmdInsertDebugUtilsLabelEXT(m_handle, &debugLabel);
		}
	}

	inline void CommandBuffer::MemoryBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask)
	{
		VkMemoryBarrier memoryBarrier = {
			VK_STRUCTURE_TYPE_MEMORY_BARRIER,
			nullptr,
			srcAccessMask,
			dstAccessMask
		};

		return PipelineBarrier(srcStageMask, dstStageMask, 0U, memoryBarrier);
	}

	inline void CommandBuffer::NextSubpass(VkSubpassContents contents)
	{
		return m_pool->GetDevice()->vkCmdNextSubpass(m_handle, contents);
	}

	inline void CommandBuffer::PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, const VkImageMemoryBarrier& imageMemoryBarrier)
	{
		return PipelineBarrier(srcStageMask, dstStageMask, dependencyFlags, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	}

	inline void CommandBuffer::PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, const VkMemoryBarrier& memoryBarrier)
	{
		return PipelineBarrier(srcStageMask, dstStageMask, dependencyFlags, 1, &memoryBarrier, 0, nullptr, 0, nullptr);
	}

	inline void CommandBuffer::PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, UInt32 memoryBarrierCount, const VkMemoryBarrier* memoryBarriers, UInt32 bufferMemoryBarrierCount, const VkBufferMemoryBarrier* bufferMemoryBarriers, UInt32 imageMemoryBarrierCount, const VkImageMemoryBarrier* imageMemoryBarriers)
	{
		return m_pool->GetDevice()->vkCmdPipelineBarrier(m_handle, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, memoryBarriers, bufferMemoryBarrierCount, bufferMemoryBarriers, imageMemoryBarrierCount, imageMemoryBarriers);
	}

	inline void CommandBuffer::SetScissor(const Recti& scissorRegion)
	{
		VkRect2D rect = {
			{
				scissorRegion.x,
				scissorRegion.y
			},
			{
				SafeCast<UInt32>(scissorRegion.width),
				SafeCast<UInt32>(scissorRegion.height)
			}
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
			VK_IMAGE_ASPECT_COLOR_BIT,
			0, //< baseMipLevel
			1, //< levelCount
			0, //< baseArrayLayer
			1  //< layerCount
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
				if (oldImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
					srcAccessMask |= VK_ACCESS_SHADER_READ_BIT;

				dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;
			case VK_IMAGE_LAYOUT_GENERAL:
			case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
			case VK_IMAGE_LAYOUT_UNDEFINED:
			default:
				dstAccessMask = 0;
				break;
		}

		return ImageBarrier(srcStageMask, dstStageMask, 0, srcAccessMask, dstAccessMask, oldImageLayout, newImageLayout, image, subresourceRange);
	}

	inline void CommandBuffer::SetViewport(const Rectf& viewport, float minDepth, float maxDepth)
	{
		VkViewport rect = {
			viewport.x,
			viewport.y,
			viewport.width,
			viewport.height,
			minDepth,
			maxDepth
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

	inline CommandBuffer& CommandBuffer::operator=(CommandBuffer&& commandBuffer) noexcept
	{
		m_lastErrorCode = commandBuffer.m_lastErrorCode;
		m_pool = commandBuffer.m_pool;

		std::swap(m_handle, commandBuffer.m_handle);

		return *this;
	}

	inline CommandBuffer::operator VkCommandBuffer() const
	{
		return m_handle;
	}
}

