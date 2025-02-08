---
title: Vk.CommandPool
description: Nothing
---

# Nz::Vk::CommandPool

Class description

## Constructors

- `CommandPool()`
- `CommandPool(`Vk::CommandPool` const&)`
- `CommandPool(`Vk::CommandPool`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::Vk::CommandBuffer` | `AllocateCommandBuffer(VkCommandBufferLevel level)` |
| `std::vector<`[`CommandBuffer`](documentation/generated/Renderer/CommandBuffer.md)`>` | `AllocateCommandBuffers(Nz::UInt32 commandBufferCount, VkCommandBufferLevel level)` |
| `bool` | `Create(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, Nz::UInt32 queueFamilyIndex, VkCommandPoolCreateFlags flags, VkAllocationCallbacks const* allocator)` |
| `bool` | `Reset(VkCommandPoolResetFlags flags)` |
| Vk::CommandPool`&` | `operator=(`Vk::CommandPool` const&)` |
| Vk::CommandPool`&` | `operator=(`Vk::CommandPool`&&)` |
