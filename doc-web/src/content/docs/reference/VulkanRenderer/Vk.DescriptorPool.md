---
title: Vk.DescriptorPool
description: Nothing
---

# Nz::Vk::DescriptorPool

Class description

## Constructors

- `DescriptorPool()`
- `DescriptorPool(`Vk::DescriptorPool` const&)`
- `DescriptorPool(`Vk::DescriptorPool`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::Vk::DescriptorSet` | `AllocateDescriptorSet(VkDescriptorSetLayout const& setLayouts)` |
| `std::vector<DescriptorSet>` | `AllocateDescriptorSets(Nz::UInt32 descriptorSetCount, VkDescriptorSetLayout const* setLayouts)` |
| `bool` | `Create(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, Nz::UInt32 maxSets, VkDescriptorPoolSize const& poolSize, VkDescriptorPoolCreateFlags flags, VkAllocationCallbacks const* allocator)` |
| `bool` | `Create(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, Nz::UInt32 maxSets, Nz::UInt32 poolSizeCount, VkDescriptorPoolSize const* poolSize, VkDescriptorPoolCreateFlags flags, VkAllocationCallbacks const* allocator)` |
| Vk::DescriptorPool`&` | `operator=(`Vk::DescriptorPool` const&)` |
| Vk::DescriptorPool`&` | `operator=(`Vk::DescriptorPool`&&)` |
