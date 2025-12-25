---
title: Vk.Pipeline
description: Nothing
---

# Nz::Vk::Pipeline

Class description

## Constructors

- `Pipeline()`
- `Pipeline(`Vk::Pipeline` const&)`
- `Pipeline(`Vk::Pipeline`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `CreateCompute(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, VkComputePipelineCreateInfo const& createInfo, VkPipelineCache cache, VkAllocationCallbacks const* allocator)` |
| `bool` | `CreateGraphics(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, VkGraphicsPipelineCreateInfo const& createInfo, VkPipelineCache cache, VkAllocationCallbacks const* allocator)` |
| `void` | `Destroy()` |
| [`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`&` | `GetDevice()` |
| `VkResult` | `GetLastErrorCode()` |
| `void` | `SetDebugName(std::string_view name)` |
| Vk::Pipeline`&` | `operator=(`Vk::Pipeline` const&)` |
| Vk::Pipeline`&` | `operator=(`Vk::Pipeline`&&)` |
