---
title: Vk.DescriptorSet
description: Nothing
---

# Nz::Vk::DescriptorSet

Class description

## Constructors

- `DescriptorSet()`
- `DescriptorSet(`Vk::DescriptorSet` const&)`
- `DescriptorSet(`Vk::DescriptorSet`&& descriptorSet)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Free()` |
| `bool` | `IsValid()` |
| `void` | `WriteCombinedImageSamplerDescriptor(Nz::UInt32 binding, VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout)` |
| `void` | `WriteCombinedImageSamplerDescriptor(Nz::UInt32 binding, VkDescriptorImageInfo const& imageInfo)` |
| `void` | `WriteCombinedImageSamplerDescriptor(Nz::UInt32 binding, Nz::UInt32 arrayElement, VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout)` |
| `void` | `WriteCombinedImageSamplerDescriptor(Nz::UInt32 binding, Nz::UInt32 arrayElement, VkDescriptorImageInfo const& imageInfo)` |
| `void` | `WriteCombinedImageSamplerDescriptors(Nz::UInt32 binding, Nz::UInt32 descriptorCount, VkDescriptorImageInfo const* imageInfo)` |
| `void` | `WriteCombinedImageSamplerDescriptors(Nz::UInt32 binding, Nz::UInt32 arrayElement, Nz::UInt32 descriptorCount, VkDescriptorImageInfo const* imageInfo)` |
| `void` | `WriteUniformDescriptor(Nz::UInt32 binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)` |
| `void` | `WriteUniformDescriptor(Nz::UInt32 binding, VkDescriptorBufferInfo const& bufferInfo)` |
| `void` | `WriteUniformDescriptor(Nz::UInt32 binding, Nz::UInt32 arrayElement, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)` |
| `void` | `WriteUniformDescriptor(Nz::UInt32 binding, Nz::UInt32 arrayElement, VkDescriptorBufferInfo const& bufferInfo)` |
| `void` | `WriteUniformDescriptors(Nz::UInt32 binding, Nz::UInt32 descriptorCount, VkDescriptorBufferInfo const* bufferInfo)` |
| `void` | `WriteUniformDescriptors(Nz::UInt32 binding, Nz::UInt32 arrayElement, Nz::UInt32 descriptorCount, VkDescriptorBufferInfo const* bufferInfo)` |
| Vk::DescriptorSet`&` | `operator=(`Vk::DescriptorSet` const&)` |
| Vk::DescriptorSet`&` | `operator=(`Vk::DescriptorSet`&& descriptorSet)` |
