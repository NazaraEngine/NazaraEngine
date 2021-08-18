// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKCOMMANDBUFFER_HPP
#define NAZARA_VULKANRENDERER_VKCOMMANDBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/VulkanRenderer/Wrapper/AutoFree.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandPool.hpp>
#include <vulkan/vulkan_core.h>

namespace Nz 
{
	namespace Vk
	{
		class CommandBuffer
		{
			friend CommandPool;

			public:
				inline CommandBuffer();
				CommandBuffer(const CommandBuffer&) = delete;
				inline CommandBuffer(CommandBuffer&& commandBuffer);
				~CommandBuffer() = default;

				inline bool Begin(const VkCommandBufferBeginInfo& info);
				inline bool Begin(VkCommandBufferUsageFlags flags = 0);
				inline bool Begin(VkCommandBufferUsageFlags flags, const VkCommandBufferInheritanceInfo& inheritanceInfo);
				inline bool Begin(VkCommandBufferUsageFlags flags, VkRenderPass renderPass, UInt32 subpass, VkFramebuffer framebuffer, bool occlusionQueryEnable, VkQueryControlFlags queryFlags, VkQueryPipelineStatisticFlags pipelineStatistics);
				inline bool Begin(VkCommandBufferUsageFlags flags, bool occlusionQueryEnable, VkQueryControlFlags queryFlags, VkQueryPipelineStatisticFlags pipelineStatistics);

				inline void BeginDebugRegion(const char* label);
				inline void BeginDebugRegion(const char* label, Nz::Color color);
				inline void BeginRenderPass(const VkRenderPassBeginInfo& beginInfo, VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);

				inline void BindDescriptorSet(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, UInt32 firstSet, const VkDescriptorSet& descriptorSets);
				inline void BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, UInt32 firstSet, UInt32 descriptorSetCount, const VkDescriptorSet* descriptorSets);
				inline void BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, UInt32 firstSet, UInt32 descriptorSetCount, const VkDescriptorSet* descriptorSets, UInt32 dynamicOffsetCount, const UInt32* dynamicOffsets);
				inline void BindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType);
				inline void BindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline);
				inline void BindVertexBuffer(UInt32 binding, const VkBuffer buffer, const VkDeviceSize offset);
				inline void BindVertexBuffers(UInt32 firstBinding, UInt32 bindingCount, const VkBuffer* buffer, const VkDeviceSize* offset);

				inline void ClearAttachment(const VkClearAttachment& attachment, const VkClearRect& rect);
				inline void ClearAttachments(UInt32 attachmentCount, const VkClearAttachment* attachments, UInt32 rectCount, const VkClearRect* rects);

				inline void ClearColorImage(VkImage image, VkImageLayout imageLayout, const VkClearColorValue& color, const VkImageSubresourceRange& range);
				inline void ClearColorImage(VkImage image, VkImageLayout imageLayout, const VkClearColorValue& color, UInt32 rangeCount, const VkImageSubresourceRange* ranges);

				inline void ClearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue& depthStencil, const VkImageSubresourceRange& range);
				inline void ClearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue& depthStencil, UInt32 rangeCount, const VkImageSubresourceRange* ranges);

				inline void CopyBuffer(VkBuffer source, VkBuffer target, UInt64 size, UInt64 sourceOffset = 0, UInt64 targetOffset = 0);
				inline void CopyBufferToImage(VkBuffer source, VkImage target, VkImageLayout targetLayout, UInt32 width, UInt32 height, UInt32 depth = 1);
				inline void CopyBufferToImage(VkBuffer source, VkImage target, VkImageLayout targetLayout, const VkImageSubresourceLayers& subresourceLayers, UInt32 width, UInt32 height, UInt32 depth = 1);

				inline void Draw(UInt32 vertexCount, UInt32 instanceCount = 1, UInt32 firstVertex = 0, UInt32 firstInstance = 0);
				inline void DrawIndexed(UInt32 indexCount, UInt32 instanceCount = 1, UInt32 firstVertex = 0, Int32 vertexOffset = 0, UInt32 firstInstance = 0);

				inline bool End();

				inline void EndDebugRegion();
				inline void EndRenderPass();

				inline void Free();

				inline CommandPool& GetPool();

				inline void ImageBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout, VkImage image, VkImageAspectFlags aspectFlags);

				inline void InsertDebugLabel(const char* label);
				inline void InsertDebugLabel(const char* label, Nz::Color color);

				inline void MemoryBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask);

				inline void NextSubpass(VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);

				inline void PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, const VkImageMemoryBarrier& imageMemoryBarrier);
				inline void PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, const VkMemoryBarrier& memoryBarrier);
				inline void PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, UInt32 memoryBarrierCount, const VkMemoryBarrier* memoryBarriers, UInt32 bufferMemoryBarrierCount, const VkBufferMemoryBarrier* bufferMemoryBarriers, UInt32 imageMemoryBarrierCount, const VkImageMemoryBarrier* imageMemoryBarriers);

				inline void SetScissor(const Recti& scissorRegion);
				inline void SetScissor(const VkRect2D& scissorRegion);
				inline void SetScissor(UInt32 firstScissor, UInt32 scissorCount, const VkRect2D* scissors);
				inline void SetViewport(const Rectf& viewport, float minDepth, float maxDepth);
				inline void SetViewport(const VkViewport& viewport);
				inline void SetViewport(UInt32 firstViewport, UInt32 viewportCount, const VkViewport* viewports);

				inline void SetImageLayout(VkImage image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout);
				inline void SetImageLayout(VkImage image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, const VkImageSubresourceRange& subresourceRange);
				inline void SetImageLayout(VkImage image, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout);
				inline void SetImageLayout(VkImage image, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, const VkImageSubresourceRange& subresourceRange);

				inline VkResult GetLastErrorCode() const;

				CommandBuffer& operator=(const CommandBuffer&) = delete;
				CommandBuffer& operator=(CommandBuffer&& commandBuffer) noexcept;

				inline operator VkCommandBuffer() const;

			private:
				inline CommandBuffer(CommandPool& pool, VkCommandBuffer handle);

				CommandPool* m_pool;
				VkCommandBuffer m_handle;
				VkResult m_lastErrorCode;
		};

		class AutoCommandBuffer : public AutoFree<CommandBuffer>
		{
			public:
				using AutoFree::AutoFree;

				operator VkCommandBuffer() const { return Get(); }
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.inl>

#endif // NAZARA_VULKANRENDERER_VKCOMMANDBUFFER_HPP
