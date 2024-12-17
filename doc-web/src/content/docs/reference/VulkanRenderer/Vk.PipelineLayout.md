---
title: Vk.PipelineLayout
description: Nothing
---

# Nz::Vk::PipelineLayout

Class description

## Constructors

- `PipelineLayout()`
- `PipelineLayout(`Vk::PipelineLayout` const&)`
- `PipelineLayout(`Vk::PipelineLayout`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Create(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, VkDescriptorSetLayout layout, VkPipelineLayoutCreateFlags flags)` |
| `bool` | `Create(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, Nz::UInt32 layoutCount, VkDescriptorSetLayout const* layouts, VkPipelineLayoutCreateFlags flags)` |
| Vk::PipelineLayout`&` | `operator=(`Vk::PipelineLayout` const&)` |
| Vk::PipelineLayout`&` | `operator=(`Vk::PipelineLayout`&&)` |
