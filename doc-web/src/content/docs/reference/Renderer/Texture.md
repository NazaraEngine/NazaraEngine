---
title: Texture
description: Nothing
---

# Nz::Texture

Class description

## Constructors

- `Texture()`
- `Texture(`Texture` const&)`
- `Texture(`Texture`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Copy(`Texture` const& source, Nz::Boxui const& srcBox, Nz::Vector3ui const& dstPos)` |
| `std::shared_ptr<`Texture`>` | `CreateView(`[`TextureViewInfo`](documentation/generated/Renderer/TextureViewInfo.md)` const& viewInfo)` |
| Texture`*` | `GetParentTexture()` |
| [`TextureInfo`](documentation/generated/Renderer/TextureInfo.md)` const&` | `GetTextureInfo()` |
| `void` | `UpdateDebugName(std::string_view name)` |
| Texture`&` | `operator=(`Texture` const&)` |
| Texture`&` | `operator=(`Texture`&&)` |
