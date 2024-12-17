---
title: Vk.DescriptorSetLayout
description: Nothing
---

# Nz::Vk::DescriptorSetLayout

Class description

## Constructors

- `DescriptorSetLayout()`
- `DescriptorSetLayout(`Vk::DescriptorSetLayout` const&)`
- `DescriptorSetLayout(`Vk::DescriptorSetLayout`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Create(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, VkDescriptorSetLayoutBinding const& binding, VkDescriptorSetLayoutCreateFlags flags, VkAllocationCallbacks const* allocator)` |
| `bool` | `Create(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, Nz::UInt32 bindingCount, VkDescriptorSetLayoutBinding const* binding, VkDescriptorSetLayoutCreateFlags flags, VkAllocationCallbacks const* allocator)` |
| Vk::DescriptorSetLayout`&` | `operator=(`Vk::DescriptorSetLayout` const&)` |
| Vk::DescriptorSetLayout`&` | `operator=(`Vk::DescriptorSetLayout`&&)` |
