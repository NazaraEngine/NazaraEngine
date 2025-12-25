---
title: VulkanCommandBuffer
description: Nothing
---

# Nz::VulkanCommandBuffer

Class description

## Constructors

- `VulkanCommandBuffer(`[`VulkanCommandPool`](documentation/generated/VulkanRenderer/VulkanCommandPool.md)`& owner, std::size_t poolIndex, std::size_t bindingIndex, `[`Vk::AutoCommandBuffer`](documentation/generated/VulkanRenderer/Vk.AutoCommandBuffer.md)` commandBuffer)`
- `VulkanCommandBuffer(`VulkanCommandBuffer` const&)`
- `VulkanCommandBuffer(`VulkanCommandBuffer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::size_t` | `GetBindingIndex()` |
| [`Vk::CommandBuffer`](documentation/generated/VulkanRenderer/Vk.CommandBuffer.md)` const&` | `GetCommandBuffer()` |
| `std::size_t` | `GetPoolIndex()` |
| [`VulkanCommandPool`](documentation/generated/VulkanRenderer/VulkanCommandPool.md)` const&` | `GetOwner()` |
| `void` | `UpdateDebugName(std::string_view name)` |
| VulkanCommandBuffer`&` | `operator=(`VulkanCommandBuffer` const&)` |
| VulkanCommandBuffer`&` | `operator=(`VulkanCommandBuffer`&&)` |
