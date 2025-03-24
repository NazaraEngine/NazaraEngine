---
title: Vk.QueueHandle
description: Nothing
---

# Nz::Vk::QueueHandle

Class description

## Constructors

- `QueueHandle()`
- `QueueHandle(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, VkQueue queue, Nz::UInt32 queueFamilyIndex)`
- `QueueHandle(`Vk::QueueHandle` const& queue)`
- `QueueHandle(`Vk::QueueHandle`&& queue)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`&` | `GetDevice()` |
| `VkResult` | `GetLastErrorCode()` |
| `Nz::UInt32` | `GetQueueFamilyIndex()` |
| `bool` | `Present(VkPresentInfoKHR const& presentInfo)` |
| `bool` | `Present(VkSwapchainKHR swapchain, Nz::UInt32 imageIndex, VkSemaphore waitSemaphore)` |
| `bool` | `Submit(VkCommandBuffer commandBuffer, VkFence signalFence)` |
| `bool` | `Submit(VkCommandBuffer commandBuffer, VkSemaphore waitSemaphore, VkPipelineStageFlags waitStage, VkSemaphore signalSemaphore, VkFence signalFence)` |
| `bool` | `Submit(VkCommandBuffer commandBuffer, std::initializer_list<VkSemaphore> waitSemaphores, std::initializer_list<VkPipelineStageFlags> waitStages, std::initializer_list<VkSemaphore> signalSemaphores, VkFence signalFence)` |
| `bool` | `Submit(Nz::UInt32 commandBufferCount, VkCommandBuffer const* commandBuffers, std::initializer_list<VkSemaphore> waitSemaphores, std::initializer_list<VkPipelineStageFlags> waitStages, std::initializer_list<VkSemaphore> signalSemaphores, VkFence signalFence)` |
| `bool` | `Submit(Nz::UInt32 commandBufferCount, VkCommandBuffer const* commandBuffers, VkSemaphore waitSemaphore, VkPipelineStageFlags waitStage, VkSemaphore signalSemaphore, VkFence signalFence)` |
| `bool` | `Submit(Nz::UInt32 commandBufferCount, VkCommandBuffer const* commandBuffers, Nz::UInt32 waitSemaphoreCount, VkSemaphore const* waitSemaphores, VkPipelineStageFlags const* waitStage, Nz::UInt32 signalSemaphoreCount, VkSemaphore const* signalSemaphores, VkFence signalFence)` |
| `bool` | `Submit(VkSubmitInfo const& submit, VkFence signalFence)` |
| `bool` | `Submit(Nz::UInt32 submitCount, VkSubmitInfo const* submits, VkFence signalFence)` |
| `bool` | `WaitIdle()` |
| Vk::QueueHandle`&` | `operator=(`Vk::QueueHandle` const& queue)` |
| Vk::QueueHandle`&` | `operator=(`Vk::QueueHandle`&&)` |
