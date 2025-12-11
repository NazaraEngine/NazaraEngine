---
title: VulkanRenderPipelineLayout
description: Nothing
---

# Nz::VulkanRenderPipelineLayout

Class description

## Constructors

- `VulkanRenderPipelineLayout()`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::ShaderBindingPtr` | `AllocateShaderBinding(Nz::UInt32 setIndex)` |
| `bool` | `Create(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, `[`RenderPipelineLayoutInfo`](documentation/generated/Renderer/RenderPipelineLayoutInfo.md)` layoutInfo)` |
| [`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`*` | `GetDevice()` |
| [`Vk::PipelineLayout`](documentation/generated/VulkanRenderer/Vk.PipelineLayout.md)` const&` | `GetPipelineLayout()` |
| `void` | `UpdateDebugName(std::string_view name)` |
