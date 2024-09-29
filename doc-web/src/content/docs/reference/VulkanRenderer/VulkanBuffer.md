---
title: VulkanBuffer
description: Nothing
---

# Nz::VulkanBuffer

Class description

## Constructors

- `VulkanBuffer(`[`VulkanDevice`](documentation/generated/VulkanRenderer/VulkanDevice.md)`& device, Nz::BufferType type, Nz::UInt64 size, Nz::BufferUsageFlags usage, void const* initialData)`
- `VulkanBuffer(`VulkanBuffer` const&)`
- `VulkanBuffer(`VulkanBuffer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Fill(void const* data, Nz::UInt64 offset, Nz::UInt64 size)` |
| `VkBuffer` | `GetBuffer()` |
| `void*` | `Map(Nz::UInt64 offset, Nz::UInt64 size)` |
| `bool` | `Unmap()` |
| `void` | `UpdateDebugName(std::string_view name)` |
| VulkanBuffer`&` | `operator=(`VulkanBuffer` const&)` |
| VulkanBuffer`&` | `operator=(`VulkanBuffer`&&)` |
