---
title: VulkanShaderBinding
description: Nothing
---

# Nz::VulkanShaderBinding

Class description

## Constructors

- `VulkanShaderBinding(`[`VulkanRenderPipelineLayout`](documentation/generated/VulkanRenderer/VulkanRenderPipelineLayout.md)`& owner, std::size_t poolIndex, std::size_t bindingIndex, `[`Vk::DescriptorSet`](documentation/generated/VulkanRenderer/Vk.DescriptorSet.md)` descriptorSet)`
- `VulkanShaderBinding(`VulkanShaderBinding` const&)`
- `VulkanShaderBinding(`VulkanShaderBinding`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::size_t` | `GetBindingIndex()` |
| [`Vk::DescriptorSet`](documentation/generated/VulkanRenderer/Vk.DescriptorSet.md)` const&` | `GetDescriptorSet()` |
| `std::size_t` | `GetPoolIndex()` |
| [`VulkanRenderPipelineLayout`](documentation/generated/VulkanRenderer/VulkanRenderPipelineLayout.md)` const&` | `GetOwner()` |
| `void` | `Update(`[`ShaderBinding::Binding`](documentation/generated/Renderer/ShaderBinding.Binding.md)` const* bindings, std::size_t bindingCount)` |
| `void` | `UpdateDebugName(std::string_view name)` |
| VulkanShaderBinding`&` | `operator=(`VulkanShaderBinding` const&)` |
| VulkanShaderBinding`&` | `operator=(`VulkanShaderBinding`&&)` |
