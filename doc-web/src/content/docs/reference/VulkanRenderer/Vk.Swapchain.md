---
title: Vk.Swapchain
description: Nothing
---

# Nz::Vk::Swapchain

Class description

## Constructors

- `Swapchain()`
- `Swapchain(`Vk::Swapchain` const&)`
- `Swapchain(`Vk::Swapchain`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `AcquireNextImage(Nz::UInt64 timeout, VkSemaphore semaphore, VkFence fence, Nz::UInt32* imageIndex)` |
| `bool` | `Create(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, VkSwapchainCreateInfoKHR const& createInfo, VkAllocationCallbacks const* allocator)` |
| [`Vk::Swapchain::Image`](documentation/generated/VulkanRenderer/Vk.Swapchain.Image.md)` const&` | `GetImage(Nz::UInt32 index)` |
| `std::vector<`[`Image`](documentation/generated/Utility/Image.md)`> const&` | `GetImages()` |
| `Nz::UInt32` | `GetImageCount()` |
| `bool` | `IsSupported()` |
| Vk::Swapchain`&` | `operator=(`Vk::Swapchain` const&)` |
| Vk::Swapchain`&` | `operator=(`Vk::Swapchain`&&)` |
