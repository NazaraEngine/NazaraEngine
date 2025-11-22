---
title: VulkanRenderImage
description: Nothing
---

# Nz::VulkanRenderImage

Class description

## Constructors

- `VulkanRenderImage(`[`VulkanSwapchain`](documentation/generated/VulkanRenderer/VulkanSwapchain.md)`& owner)`
- `VulkanRenderImage(`VulkanRenderImage` const&)`
- `VulkanRenderImage(`VulkanRenderImage`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Execute(FunctionRef<void (Nz::CommandBufferBuilder &)> const& callback, Nz::QueueTypeFlags queueTypeFlags)` |
| [`Vk::Fence`](documentation/generated/VulkanRenderer/Vk.Fence.md)`&` | `GetInFlightFence()` |
| [`Vk::Semaphore`](documentation/generated/VulkanRenderer/Vk.Semaphore.md)`&` | `GetImageAvailableSemaphore()` |
| `Nz::UInt32` | `GetImageIndex()` |
| [`Vk::Semaphore`](documentation/generated/VulkanRenderer/Vk.Semaphore.md)`&` | `GetRenderFinishedSemaphore()` |
| [`VulkanUploadPool`](documentation/generated/VulkanRenderer/VulkanUploadPool.md)`&` | `GetUploadPool()` |
| `void` | `Present()` |
| `void` | `Reset(Nz::UInt32 imageIndex)` |
| `void` | `SubmitCommandBuffer(`[`CommandBuffer`](documentation/generated/Renderer/CommandBuffer.md)`* commandBuffer, Nz::QueueTypeFlags queueTypeFlags)` |
| `void` | `SubmitCommandBuffer(VkCommandBuffer commandBuffer, Nz::QueueTypeFlags queueTypeFlags)` |
| VulkanRenderImage`&` | `operator=(`VulkanRenderImage` const&)` |
| VulkanRenderImage`&` | `operator=(`VulkanRenderImage`&&)` |
