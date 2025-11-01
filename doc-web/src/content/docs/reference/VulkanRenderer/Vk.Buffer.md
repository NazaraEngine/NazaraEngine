---
title: Vk.Buffer
description: Nothing
---

# Nz::Vk::Buffer

Class description

## Constructors

- `Buffer()`
- `Buffer(`Vk::Buffer` const&)`
- `Buffer(`Vk::Buffer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `BindBufferMemory(VkDeviceMemory memory, VkDeviceSize offset)` |
| `bool` | `Create(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, VkBufferCreateFlags flags, VkDeviceSize size, VkBufferUsageFlags usage, VkAllocationCallbacks const* allocator)` |
| `VkMemoryRequirements` | `GetMemoryRequirements()` |
| Vk::Buffer`&` | `operator=(`Vk::Buffer` const&)` |
| Vk::Buffer`&` | `operator=(`Vk::Buffer`&&)` |
