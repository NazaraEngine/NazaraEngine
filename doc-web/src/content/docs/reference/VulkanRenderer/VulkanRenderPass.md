---
title: VulkanRenderPass
description: Nothing
---

# Nz::VulkanRenderPass

Class description

## Constructors

- `VulkanRenderPass(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, std::vector<Attachment> attachments, std::vector<SubpassDescription> subpassDescriptions, std::vector<SubpassDependency> subpassDependencies)`
- `VulkanRenderPass(`VulkanRenderPass` const&)`
- `VulkanRenderPass(`VulkanRenderPass`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`Vk::RenderPass`](documentation/generated/VulkanRenderer/Vk.RenderPass.md)`&` | `GetRenderPass()` |
| [`Vk::RenderPass`](documentation/generated/VulkanRenderer/Vk.RenderPass.md)` const&` | `GetRenderPass()` |
| `void` | `UpdateDebugName(std::string_view name)` |
| VulkanRenderPass`&` | `operator=(`VulkanRenderPass` const&)` |
| VulkanRenderPass`&` | `operator=(`VulkanRenderPass`&&)` |
