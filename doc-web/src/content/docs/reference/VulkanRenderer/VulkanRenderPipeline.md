---
title: VulkanRenderPipeline
description: Nothing
---

# Nz::VulkanRenderPipeline

Class description

## Constructors

- `VulkanRenderPipeline(`[`VulkanDevice`](documentation/generated/VulkanRenderer/VulkanDevice.md)`& device, `[`RenderPipelineInfo`](documentation/generated/Renderer/RenderPipelineInfo.md)` pipelineInfo)`
- `VulkanRenderPipeline(`VulkanRenderPipeline` const&)`
- `VulkanRenderPipeline(`VulkanRenderPipeline`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `VkPipeline` | `Get(`[`VulkanRenderPass`](documentation/generated/VulkanRenderer/VulkanRenderPass.md)` const& renderPass, std::size_t subpassIndex)` |
| [`RenderPipelineInfo`](documentation/generated/Renderer/RenderPipelineInfo.md)` const&` | `GetPipelineInfo()` |
| `void` | `UpdateDebugName(std::string_view name)` |
| VulkanRenderPipeline`&` | `operator=(`VulkanRenderPipeline` const&)` |
| VulkanRenderPipeline`&` | `operator=(`VulkanRenderPipeline`&&)` |
