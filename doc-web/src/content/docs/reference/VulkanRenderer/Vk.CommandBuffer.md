---
title: Vk.CommandBuffer
description: Nothing
---

# Nz::Vk::CommandBuffer

Class description

## Constructors

- `CommandBuffer()`
- `CommandBuffer(`[`Vk::CommandPool`](documentation/generated/VulkanRenderer/Vk.CommandPool.md)`& pool, VkCommandBuffer handle)`
- `CommandBuffer(`Vk::CommandBuffer` const&)`
- `CommandBuffer(`Vk::CommandBuffer`&& commandBuffer)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Begin(VkCommandBufferBeginInfo const& info)` |
| `bool` | `Begin(VkCommandBufferUsageFlags flags)` |
| `bool` | `Begin(VkCommandBufferUsageFlags flags, VkCommandBufferInheritanceInfo const& inheritanceInfo)` |
| `bool` | `Begin(VkCommandBufferUsageFlags flags, VkRenderPass renderPass, Nz::UInt32 subpass, VkFramebuffer framebuffer, bool occlusionQueryEnable, VkQueryControlFlags queryFlags, VkQueryPipelineStatisticFlags pipelineStatistics)` |
| `bool` | `Begin(VkCommandBufferUsageFlags flags, bool occlusionQueryEnable, VkQueryControlFlags queryFlags, VkQueryPipelineStatisticFlags pipelineStatistics)` |
| `void` | `BeginDebugRegion(char const* label)` |
| `void` | `BeginDebugRegion(char const* label, `[`Color`](documentation/generated/Core/Color.md)` color)` |
| `void` | `BeginRenderPass(VkRenderPassBeginInfo const& beginInfo, VkSubpassContents contents)` |
| `void` | `BindDescriptorSet(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, Nz::UInt32 firstSet, VkDescriptorSet const& descriptorSets)` |
| `void` | `BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, Nz::UInt32 firstSet, Nz::UInt32 descriptorSetCount, VkDescriptorSet const* descriptorSets)` |
| `void` | `BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, Nz::UInt32 firstSet, Nz::UInt32 descriptorSetCount, VkDescriptorSet const* descriptorSets, Nz::UInt32 dynamicOffsetCount, Nz::UInt32 const* dynamicOffsets)` |
| `void` | `BindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType)` |
| `void` | `BindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline)` |
| `void` | `BindVertexBuffer(Nz::UInt32 binding, VkBuffer const buffer, VkDeviceSize const offset)` |
| `void` | `BindVertexBuffers(Nz::UInt32 firstBinding, Nz::UInt32 bindingCount, VkBuffer const* buffer, VkDeviceSize const* offset)` |
| `void` | `BlitImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, VkImageBlit const& region, VkFilter filter)` |
| `void` | `BlitImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, Nz::UInt32 regionCount, VkImageBlit const* regions, VkFilter filter)` |
| `void` | `ClearAttachment(VkClearAttachment const& attachment, VkClearRect const& rect)` |
| `void` | `ClearAttachments(Nz::UInt32 attachmentCount, VkClearAttachment const* attachments, Nz::UInt32 rectCount, VkClearRect const* rects)` |
| `void` | `ClearColorImage(VkImage image, VkImageLayout imageLayout, VkClearColorValue const& color, VkImageSubresourceRange const& range)` |
| `void` | `ClearColorImage(VkImage image, VkImageLayout imageLayout, VkClearColorValue const& color, Nz::UInt32 rangeCount, VkImageSubresourceRange const* ranges)` |
| `void` | `ClearDepthStencilImage(VkImage image, VkImageLayout imageLayout, VkClearDepthStencilValue const& depthStencil, VkImageSubresourceRange const& range)` |
| `void` | `ClearDepthStencilImage(VkImage image, VkImageLayout imageLayout, VkClearDepthStencilValue const& depthStencil, Nz::UInt32 rangeCount, VkImageSubresourceRange const* ranges)` |
| `void` | `CopyBuffer(VkBuffer source, VkBuffer target, Nz::UInt64 size, Nz::UInt64 sourceOffset, Nz::UInt64 targetOffset)` |
| `void` | `CopyBufferToImage(VkBuffer source, VkImage target, VkImageLayout targetLayout, Nz::UInt32 width, Nz::UInt32 height, Nz::UInt32 depth)` |
| `void` | `CopyBufferToImage(VkBuffer source, VkImage target, VkImageLayout targetLayout, VkImageSubresourceLayers const& subresourceLayers, Nz::Int32 x, Nz::Int32 y, Nz::Int32 z, Nz::UInt32 width, Nz::UInt32 height, Nz::UInt32 depth)` |
| `void` | `CopyBufferToImage(VkBuffer source, VkImage target, VkImageLayout targetLayout, VkImageSubresourceLayers const& subresourceLayers, Nz::UInt32 width, Nz::UInt32 height, Nz::UInt32 depth)` |
| `void` | `CopyBufferToImage(VkBuffer source, VkImage target, VkImageLayout targetLayout, VkBufferImageCopy const& region)` |
| `void` | `CopyBufferToImage(VkBuffer source, VkImage target, VkImageLayout targetLayout, Nz::UInt32 regionCount, VkBufferImageCopy const* regions)` |
| `void` | `CopyImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, VkImageCopy const& region)` |
| `void` | `CopyImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, Nz::UInt32 regionCount, VkImageCopy const* regions)` |
| `void` | `Dispatch(Nz::UInt32 groupCountX, Nz::UInt32 groupCountY, Nz::UInt32 groupCountZ)` |
| `void` | `Draw(Nz::UInt32 vertexCount, Nz::UInt32 instanceCount, Nz::UInt32 firstVertex, Nz::UInt32 firstInstance)` |
| `void` | `DrawIndexed(Nz::UInt32 indexCount, Nz::UInt32 instanceCount, Nz::UInt32 firstVertex, Nz::Int32 vertexOffset, Nz::UInt32 firstInstance)` |
| `bool` | `End()` |
| `void` | `EndDebugRegion()` |
| `void` | `EndRenderPass()` |
| `void` | `Free()` |
| `VkResult` | `GetLastErrorCode()` |
| [`Vk::CommandPool`](documentation/generated/VulkanRenderer/Vk.CommandPool.md)`&` | `GetPool()` |
| `void` | `ImageBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout, VkImage image, VkImageSubresourceRange const& subresourceRange)` |
| `void` | `InsertDebugLabel(char const* label)` |
| `void` | `InsertDebugLabel(char const* label, `[`Color`](documentation/generated/Core/Color.md)` color)` |
| `void` | `MemoryBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask)` |
| `void` | `NextSubpass(VkSubpassContents contents)` |
| `void` | `PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, VkImageMemoryBarrier const& imageMemoryBarrier)` |
| `void` | `PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, VkMemoryBarrier const& memoryBarrier)` |
| `void` | `PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, Nz::UInt32 memoryBarrierCount, VkMemoryBarrier const* memoryBarriers, Nz::UInt32 bufferMemoryBarrierCount, VkBufferMemoryBarrier const* bufferMemoryBarriers, Nz::UInt32 imageMemoryBarrierCount, VkImageMemoryBarrier const* imageMemoryBarriers)` |
| `void` | `SetScissor(Nz::Recti const& scissorRegion)` |
| `void` | `SetScissor(VkRect2D const& scissorRegion)` |
| `void` | `SetScissor(Nz::UInt32 firstScissor, Nz::UInt32 scissorCount, VkRect2D const* scissors)` |
| `void` | `SetViewport(Nz::Rectf const& viewport, float minDepth, float maxDepth)` |
| `void` | `SetViewport(VkViewport const& viewport)` |
| `void` | `SetViewport(Nz::UInt32 firstViewport, Nz::UInt32 viewportCount, VkViewport const* viewports)` |
| `void` | `SetImageLayout(VkImage image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout)` |
| `void` | `SetImageLayout(VkImage image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkImageSubresourceRange const& subresourceRange)` |
| `void` | `SetImageLayout(VkImage image, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout)` |
| `void` | `SetImageLayout(VkImage image, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkImageSubresourceRange const& subresourceRange)` |
| Vk::CommandBuffer`&` | `operator=(`Vk::CommandBuffer` const&)` |
| Vk::CommandBuffer`&` | `operator=(`Vk::CommandBuffer`&& commandBuffer)` |
