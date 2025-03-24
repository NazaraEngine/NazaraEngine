---
title: TexturePropertyHandler
description: Nothing
---

# Nz::TexturePropertyHandler

Class description

## Constructors

- `TexturePropertyHandler(std::string propertyName)`
- `TexturePropertyHandler(std::string propertyName, std::string optionName)`
- `TexturePropertyHandler(std::string propertyName, std::string samplerTag, std::string optionName)`
- `TexturePropertyHandler(`TexturePropertyHandler` const&)`
- `TexturePropertyHandler(`TexturePropertyHandler`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `NeedsUpdateOnTextureUpdate(std::size_t updatedPropertyIndex)` |
| `void` | `Setup(`[`Material`](documentation/generated/Graphics/Material.md)` const& material, `[`ShaderReflection`](documentation/generated/Graphics/ShaderReflection.md)` const& reflection)` |
| `void` | `Update(`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`& materialInstance)` |
| TexturePropertyHandler`&` | `operator=(`TexturePropertyHandler` const&)` |
| TexturePropertyHandler`&` | `operator=(`TexturePropertyHandler`&&)` |
