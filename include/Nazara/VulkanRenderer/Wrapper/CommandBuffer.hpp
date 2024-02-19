// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_COMMANDBUFFER_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_COMMANDBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/VulkanRenderer/Wrapper/AutoFree.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandPool.hpp>
#include <vulkan/vulkan_core.h>

namespace Nz::Vk
{
	class CommandBuffer
	{
		friend CommandPool;

		public:
			inline CommandBuffer();
			inline CommandBuffer(CommandPool& pool, VkCommandBuffer handle);
			CommandBuffer(const CommandBuffer&) = delete;
			inline CommandBuffer(CommandBuffer&& commandBuffer) noexcept;
			~CommandBuffer() = default;

			inline bool Begin(const VkCommandBufferBeginInfo& info);
			inline bool Begin(VkCommandBufferUsageFlags flags = 0);
			inline bool Begin(VkCommandBufferUsageFlags flags, const VkCommandBufferInheritanceInfo& inheritanceInfo);
			inline bool Begin(VkCommandBufferUsageFlags flags, VkRenderPass renderPass, UInt32 subpass, VkFramebuffer framebuffer, bool occlusionQueryEnable, VkQueryControlFlags queryFlags, VkQueryPipelineStatisticFlags pipelineStatistics);
			inline bool Begin(VkCommandBufferUsageFlags flags, bool occlusionQueryEnable, VkQueryControlFlags queryFlags, VkQueryPipelineStatisticFlags pipelineStatistics);

			inline void BeginDebugRegion(const char* label);
			inline void BeginDebugRegion(const char* label, Color color);
			inline void BeginRenderPass(const VkRenderPassBeginInfo& beginInfo, VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);

			inline void BindDescriptorSet(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, UInt32 firstSet, const VkDescriptorSet& descriptorSets);
			inline void BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, UInt32 firstSet, UInt32 descriptorSetCount, const VkDescriptorSet* descriptorSets);
			inline void BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, UInt32 firstSet, UInt32 descriptorSetCount, const VkDescriptorSet* descriptorSets, UInt32 dynamicOffsetCount, const UInt32* dynamicOffsets);
			inline void BindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType);
			inline void BindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline);
			inline void BindVertexBuffer(UInt32 binding, const VkBuffer buffer, const VkDeviceSize offset);
			inline void BindVertexBuffers(UInt32 firstBinding, UInt32 bindingCount, const VkBuffer* buffer, const VkDeviceSize* offset);

			inline void BlitImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, const VkImageBlit& region, VkFilter filter);
			inline void BlitImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, UInt32 regionCount, const VkImageBlit* regions, VkFilter filter);

			inline void ClearAttachment(const VkClearAttachment& attachment, const VkClearRect& rect);
			inline void ClearAttachments(UInt32 attachmentCount, const VkClearAttachment* attachments, UInt32 rectCount, const VkClearRect* rects);

			inline void ClearColorImage(VkImage image, VkImageLayout imageLayout, const VkClearColorValue& color, const VkImageSubresourceRange& range);
			inline void ClearColorImage(VkImage image, VkImageLayout imageLayout, const VkClearColorValue& color, UInt32 rangeCount, const VkImageSubresourceRange* ranges);

			inline void ClearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue& depthStencil, const VkImageSubresourceRange& range);
			inline void ClearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue& depthStencil, UInt32 rangeCount, const VkImageSubresourceRange* ranges);

			inline void CopyBuffer(VkBuffer source, VkBuffer target, UInt64 size, UInt64 sourceOffset = 0, UInt64 targetOffset = 0);
			inline void CopyBufferToImage(VkBuffer source, VkImage target, VkImageLayout targetLayout, UInt32 width, UInt32 height, UInt32 depth = 1);
			inline void CopyBufferToImage(VkBuffer source, VkImage target, VkImageLayout targetLayout, const VkImageSubresourceLayers& subresourceLayers, Int32 x, Int32 y, Int32 z, UInt32 width, UInt32 height, UInt32 depth);
			inline void CopyBufferToImage(VkBuffer source, VkImage target, VkImageLayout targetLayout, const VkImageSubresourceLayers& subresourceLayers, UInt32 width, UInt32 height, UInt32 depth = 1);
			inline void CopyBufferToImage(VkBuffer source, VkImage target, VkImageLayout targetLayout, const VkBufferImageCopy& region);
			inline void CopyBufferToImage(VkBuffer source, VkImage target, VkImageLayout targetLayout, UInt32 regionCount, const VkBufferImageCopy* regions);
			inline void CopyImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, const VkImageCopy& region);
			inline void CopyImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, UInt32 regionCount, const VkImageCopy* regions);

			inline void Dispatch(UInt32 groupCountX, UInt32 groupCountY, UInt32 groupCountZ);

			inline void Draw(UInt32 vertexCount, UInt32 instanceCount = 1, UInt32 firstVertex = 0, UInt32 firstInstance = 0);
			inline void DrawIndexed(UInt32 indexCount, UInt32 instanceCount = 1, UInt32 firstVertex = 0, Int32 vertexOffset = 0, UInt32 firstInstance = 0);

			inline bool End();

			inline void EndDebugRegion();
			inline void EndRenderPass();

			inline void Free();

			inline VkResult GetLastErrorCode() const;
			inline CommandPool& GetPool();

			inline void ImageBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout, VkImage image, const VkImageSubresourceRange& subresourceRange);

			inline void InsertDebugLabel(const char* label);
			inline void InsertDebugLabel(const char* label, Color color);

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

			CommandBuffer& operator=(const CommandBuffer&) = delete;
			CommandBuffer& operator=(CommandBuffer&& commandBuffer) noexcept;

			inline operator VkCommandBuffer() const;

		private:
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

#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_COMMANDBUFFER_HPP
