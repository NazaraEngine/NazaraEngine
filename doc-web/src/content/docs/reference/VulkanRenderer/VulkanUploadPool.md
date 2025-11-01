---
title: VulkanUploadPool
description: Nothing
---

# Nz::VulkanUploadPool

Class description

## Constructors

- `VulkanUploadPool(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, Nz::UInt64 blockSize)`
- `VulkanUploadPool(`VulkanUploadPool` const&)`
- `VulkanUploadPool(`VulkanUploadPool`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`VulkanUploadPool::VulkanAllocation`](documentation/generated/VulkanRenderer/VulkanUploadPool.VulkanAllocation.md)`&` | `Allocate(Nz::UInt64 size)` |
| [`VulkanUploadPool::VulkanAllocation`](documentation/generated/VulkanRenderer/VulkanUploadPool.VulkanAllocation.md)`&` | `Allocate(Nz::UInt64 size, Nz::UInt64 alignment)` |
| `void` | `Reset()` |
| VulkanUploadPool`&` | `operator=(`VulkanUploadPool` const&)` |
| VulkanUploadPool`&` | `operator=(`VulkanUploadPool`&&)` |
