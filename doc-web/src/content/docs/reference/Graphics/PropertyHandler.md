---
title: PropertyHandler
description: Nothing
---

# Nz::PropertyHandler

Class description

## Constructors

- `PropertyHandler()`
- `PropertyHandler(`PropertyHandler` const&)`
- `PropertyHandler(`PropertyHandler`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `NeedsUpdateOnTextureUpdate(std::size_t updatedTexturePropertyIndex)` |
| `bool` | `NeedsUpdateOnValueUpdate(std::size_t updatedValuePropertyIndex)` |
| `void` | `Setup(`[`Material`](documentation/generated/Graphics/Material.md)` const& material, `[`ShaderReflection`](documentation/generated/Graphics/ShaderReflection.md)` const& reflection)` |
| `void` | `Update(`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`& materialInstance)` |
| PropertyHandler`&` | `operator=(`PropertyHandler` const&)` |
| PropertyHandler`&` | `operator=(`PropertyHandler`&&)` |
