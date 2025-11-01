---
title: Vk.Fence
description: Nothing
---

# Nz::Vk::Fence

Class description

## Constructors

- `Fence()`
- `Fence(`Vk::Fence` const&)`
- `Fence(`Vk::Fence`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Create(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, VkFenceCreateFlags flags, VkAllocationCallbacks const* allocator)` |
| `bool` | `Reset()` |
| `bool` | `Wait()` |
| `bool` | `Wait(Nz::UInt64 timeout, bool* didTimeout)` |
| Vk::Fence`&` | `operator=(`Vk::Fence` const&)` |
| Vk::Fence`&` | `operator=(`Vk::Fence`&&)` |
