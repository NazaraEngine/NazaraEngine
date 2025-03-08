---
title: VulkanCommandPool
description: Nothing
---

# Nz::VulkanCommandPool

Class description

## Constructors

- `VulkanCommandPool(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, Nz::QueueType queueType)`
- `VulkanCommandPool(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, Nz::UInt32 queueFamilyIndex)`
- `VulkanCommandPool(`VulkanCommandPool` const&)`
- `VulkanCommandPool(`VulkanCommandPool`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::CommandBufferPtr` | `BuildCommandBuffer(std::function<void (CommandBufferBuilder &)> const& callback)` |
| `void` | `UpdateDebugName(std::string_view name)` |
| VulkanCommandPool`&` | `operator=(`VulkanCommandPool` const&)` |
| VulkanCommandPool`&` | `operator=(`VulkanCommandPool`&&)` |
