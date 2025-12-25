---
title: VulkanSwapchain
description: Nothing
---

# Nz::VulkanSwapchain

Class description

## Constructors

- `VulkanSwapchain(`[`VulkanDevice`](documentation/generated/VulkanRenderer/VulkanDevice.md)`& device, `[`WindowHandle`](documentation/generated/Platform/WindowHandle.md)` windowHandle, Nz::Vector2ui const& windowSize, `[`SwapchainParameters`](documentation/generated/Renderer/SwapchainParameters.md)` const& parameters)`
- `VulkanSwapchain(`VulkanSwapchain` const&)`
- `VulkanSwapchain(`VulkanSwapchain`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::RenderFrame` | `AcquireFrame()` |
| `std::shared_ptr<`[`CommandPool`](documentation/generated/Renderer/CommandPool.md)`>` | `CreateCommandPool(Nz::QueueType queueType)` |
| [`VulkanWindowFramebuffer`](documentation/generated/VulkanRenderer/VulkanWindowFramebuffer.md)` const&` | `GetFramebuffer(std::size_t i)` |
| `std::size_t` | `GetFramebufferCount()` |
| [`VulkanDevice`](documentation/generated/VulkanRenderer/VulkanDevice.md)`&` | `GetDevice()` |
| [`VulkanDevice`](documentation/generated/VulkanRenderer/VulkanDevice.md)` const&` | `GetDevice()` |
| [`Vk::QueueHandle`](documentation/generated/VulkanRenderer/Vk.QueueHandle.md)`&` | `GetGraphicsQueue()` |
| [`VulkanRenderPass`](documentation/generated/VulkanRenderer/VulkanRenderPass.md)` const&` | `GetRenderPass()` |
| `Nz::Vector2ui const&` | `GetSize()` |
| [`Vk::Swapchain`](documentation/generated/VulkanRenderer/Vk.Swapchain.md)` const&` | `GetSwapchain()` |
| `void` | `NotifyResize(Nz::Vector2ui const& newSize)` |
| `void` | `Present(Nz::UInt32 imageIndex, VkSemaphore waitSemaphore)` |
| [`TransientResources`](documentation/generated/Renderer/TransientResources.md)`&` | `Transient()` |
| VulkanSwapchain`&` | `operator=(`VulkanSwapchain` const&)` |
| VulkanSwapchain`&` | `operator=(`VulkanSwapchain`&&)` |
