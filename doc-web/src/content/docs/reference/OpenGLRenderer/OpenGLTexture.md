---
title: OpenGLTexture
description: Nothing
---

# Nz::OpenGLTexture

Class description

## Constructors

- `OpenGLTexture(`[`OpenGLDevice`](documentation/generated/OpenGLRenderer/OpenGLDevice.md)`& device, `[`TextureInfo`](documentation/generated/Renderer/TextureInfo.md)` const& textureInfo)`
- `OpenGLTexture(`[`OpenGLDevice`](documentation/generated/OpenGLRenderer/OpenGLDevice.md)`& device, `[`TextureInfo`](documentation/generated/Renderer/TextureInfo.md)` const& textureInfo, void const* initialData, bool buildMipmaps, unsigned int srcWidth, unsigned int srcHeight)`
- `OpenGLTexture(std::shared_ptr<`OpenGLTexture`> parentTexture, `[`TextureViewInfo`](documentation/generated/Renderer/TextureViewInfo.md)` const& viewInfo)`
- `OpenGLTexture(`OpenGLTexture` const&)`
- `OpenGLTexture(`OpenGLTexture`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Copy(`[`Texture`](documentation/generated/Renderer/Texture.md)` const& source, Nz::Boxui const& srcBox, Nz::Vector3ui const& dstPos)` |
| `std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`>` | `CreateView(`[`TextureViewInfo`](documentation/generated/Renderer/TextureViewInfo.md)` const& viewInfo)` |
| `void` | `GenerateMipmaps(Nz::UInt8 baseLevel, Nz::UInt8 levelCount)` |
| `Nz::PixelFormat` | `GetFormat()` |
| `Nz::UInt8` | `GetLevelCount()` |
| OpenGLTexture`*` | `GetParentTexture()` |
| `Nz::Vector3ui` | `GetSize(Nz::UInt8 level)` |
| [`GL::Texture`](documentation/generated/OpenGLRenderer/GL.Texture.md)` const&` | `GetTexture()` |
| [`TextureInfo`](documentation/generated/Renderer/TextureInfo.md)` const&` | `GetTextureInfo()` |
| [`TextureViewInfo`](documentation/generated/Renderer/TextureViewInfo.md)` const&` | `GetTextureViewInfo()` |
| `Nz::ImageType` | `GetType()` |
| `bool` | `RequiresTextureViewEmulation()` |
| `bool` | `Update(void const* ptr, Nz::Boxui const& box, unsigned int srcWidth, unsigned int srcHeight, Nz::UInt8 level)` |
| `void` | `UpdateDebugName(std::string_view name)` |
| OpenGLTexture`&` | `operator=(`OpenGLTexture` const&)` |
| OpenGLTexture`&` | `operator=(`OpenGLTexture`&&)` |
