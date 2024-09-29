---
title: Vk.DeviceMemory
description: Nothing
---

# Nz::Vk::DeviceMemory

Class description

## Constructors

- `DeviceMemory()`
- `DeviceMemory(`Vk::DeviceMemory` const&)`
- `DeviceMemory(`Vk::DeviceMemory`&& memory)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Create(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, VkDeviceSize size, Nz::UInt32 memoryType, VkAllocationCallbacks const* allocator)` |
| `bool` | `Create(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, VkDeviceSize size, Nz::UInt32 typeBits, VkFlags properties, VkAllocationCallbacks const* allocator)` |
| `bool` | `FlushMemory()` |
| `bool` | `FlushMemory(Nz::UInt64 offset, Nz::UInt64 size)` |
| `void*` | `GetMappedPointer()` |
| `bool` | `Map(VkMemoryMapFlags flags)` |
| `bool` | `Map(VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags)` |
| `void` | `Unmap()` |
| Vk::DeviceMemory`&` | `operator=(`Vk::DeviceMemory` const&)` |
| Vk::DeviceMemory`&` | `operator=(`Vk::DeviceMemory`&&)` |
