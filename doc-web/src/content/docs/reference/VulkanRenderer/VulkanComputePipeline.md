---
title: VulkanComputePipeline
description: Nothing
---

# Nz::VulkanComputePipeline

Class description

## Constructors

- `VulkanComputePipeline(`[`VulkanDevice`](documentation/generated/VulkanRenderer/VulkanDevice.md)`& device, `[`ComputePipelineInfo`](documentation/generated/Renderer/ComputePipelineInfo.md)` pipelineInfo)`
- `VulkanComputePipeline(`VulkanComputePipeline` const&)`
- `VulkanComputePipeline(`VulkanComputePipeline`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`Vk::Pipeline`](documentation/generated/VulkanRenderer/Vk.Pipeline.md)` const&` | `GetPipeline()` |
| [`ComputePipelineInfo`](documentation/generated/Renderer/ComputePipelineInfo.md)` const&` | `GetPipelineInfo()` |
| `void` | `UpdateDebugName(std::string_view name)` |
| VulkanComputePipeline`&` | `operator=(`VulkanComputePipeline` const&)` |
| VulkanComputePipeline`&` | `operator=(`VulkanComputePipeline`&&)` |
