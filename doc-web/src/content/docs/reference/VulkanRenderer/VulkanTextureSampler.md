---
title: VulkanTextureSampler
description: Nothing
---

# Nz::VulkanTextureSampler

Class description

## Constructors

- `VulkanTextureSampler(`[`VulkanDevice`](documentation/generated/VulkanRenderer/VulkanDevice.md)`& device, `[`TextureSamplerInfo`](documentation/generated/Renderer/TextureSamplerInfo.md)` samplerInfo)`
- `VulkanTextureSampler(`VulkanTextureSampler` const&)`
- `VulkanTextureSampler(`VulkanTextureSampler`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `VkSampler` | `GetSampler()` |
| `void` | `UpdateDebugName(std::string_view name)` |
| VulkanTextureSampler`&` | `operator=(`VulkanTextureSampler` const&)` |
| VulkanTextureSampler`&` | `operator=(`VulkanTextureSampler`&&)` |
