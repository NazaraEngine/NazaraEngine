---
title: VulkanTextureFramebuffer
description: Nothing
---

# Nz::VulkanTextureFramebuffer

Class description

## Constructors

- `VulkanTextureFramebuffer(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, unsigned int width, unsigned int height, std::shared_ptr<`[`RenderPass`](documentation/generated/Renderer/RenderPass.md)`> const& renderPass, std::vector<std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`>> const& attachments)`
- `VulkanTextureFramebuffer(`VulkanTextureFramebuffer` const&)`
- `VulkanTextureFramebuffer(`VulkanTextureFramebuffer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`Vk::Framebuffer`](documentation/generated/VulkanRenderer/Vk.Framebuffer.md)`&` | `GetFramebuffer()` |
| [`Vk::Framebuffer`](documentation/generated/VulkanRenderer/Vk.Framebuffer.md)` const&` | `GetFramebuffer()` |
| VulkanTextureFramebuffer`&` | `operator=(`VulkanTextureFramebuffer` const&)` |
| VulkanTextureFramebuffer`&` | `operator=(`VulkanTextureFramebuffer`&&)` |
