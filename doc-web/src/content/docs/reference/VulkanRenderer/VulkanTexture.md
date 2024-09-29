---
title: VulkanTexture
description: Nothing
---

# Nz::VulkanTexture

Class description

## Constructors

- `VulkanTexture(`[`VulkanDevice`](documentation/generated/VulkanRenderer/VulkanDevice.md)`& device, `[`TextureInfo`](documentation/generated/Renderer/TextureInfo.md)` const& textureInfo)`
- `VulkanTexture(`[`VulkanDevice`](documentation/generated/VulkanRenderer/VulkanDevice.md)`& device, `[`TextureInfo`](documentation/generated/Renderer/TextureInfo.md)` const& textureInfo, void const* initialData, bool buildMipmaps, unsigned int srcWidth, unsigned int srcHeight)`
- `VulkanTexture(std::shared_ptr<`VulkanTexture`> parentTexture, `[`TextureViewInfo`](documentation/generated/Renderer/TextureViewInfo.md)` const& viewInfo)`
- `VulkanTexture(`VulkanTexture` const&)`
- `VulkanTexture(`VulkanTexture`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `VkImageSubresourceLayers` | `BuildSubresourceLayers(Nz::UInt32 level)` |
| `VkImageSubresourceLayers` | `BuildSubresourceLayers(Nz::UInt32 level, Nz::UInt32 baseLayer, Nz::UInt32 layerCount)` |
| `VkImageSubresourceRange` | `BuildSubresourceRange(Nz::UInt32 baseLevel, Nz::UInt32 levelCount)` |
| `VkImageSubresourceRange` | `BuildSubresourceRange(Nz::UInt32 baseLevel, Nz::UInt32 levelCount, Nz::UInt32 baseLayer, Nz::UInt32 layerCount)` |
| `bool` | `Copy(`[`Texture`](documentation/generated/Renderer/Texture.md)` const& source, Nz::Boxui const& srcBox, Nz::Vector3ui const& dstPos)` |
| `std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`>` | `CreateView(`[`TextureViewInfo`](documentation/generated/Renderer/TextureViewInfo.md)` const& viewInfo)` |
| `Nz::PixelFormat` | `GetFormat()` |
| `VkImage` | `GetImage()` |
| `VkImageView` | `GetImageView()` |
| `Nz::UInt8` | `GetLevelCount()` |
| VulkanTexture`*` | `GetParentTexture()` |
| `Nz::Vector3ui` | `GetSize(Nz::UInt8 level)` |
| `VkImageSubresourceRange const&` | `GetSubresourceRange()` |
| [`TextureInfo`](documentation/generated/Renderer/TextureInfo.md)` const&` | `GetTextureInfo()` |
| `Nz::ImageType` | `GetType()` |
| `bool` | `Update(void const* ptr, Nz::Boxui const& box, unsigned int srcWidth, unsigned int srcHeight, Nz::UInt8 level)` |
| `bool` | `Update(`[`Vk::CommandBuffer`](documentation/generated/VulkanRenderer/Vk.CommandBuffer.md)`& commandBuffer, std::unique_ptr<`[`VulkanBuffer`](documentation/generated/VulkanRenderer/VulkanBuffer.md)`>& uploadBuffer, void const* ptr, Nz::Boxui const& box, unsigned int srcWidth, unsigned int srcHeight, Nz::UInt8 level)` |
| `void` | `UpdateDebugName(std::string_view name)` |
| VulkanTexture`&` | `operator=(`VulkanTexture` const&)` |
| VulkanTexture`&` | `operator=(`VulkanTexture`&&)` |
