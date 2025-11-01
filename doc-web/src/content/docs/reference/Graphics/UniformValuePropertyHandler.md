---
title: UniformValuePropertyHandler
description: Nothing
---

# Nz::UniformValuePropertyHandler

Class description

## Constructors

- `UniformValuePropertyHandler(std::string propertyName, std::string blockTag)`
- `UniformValuePropertyHandler(std::string propertyName, std::string memberTag, std::string blockTag)`
- `UniformValuePropertyHandler(`UniformValuePropertyHandler` const&)`
- `UniformValuePropertyHandler(`UniformValuePropertyHandler`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `NeedsUpdateOnValueUpdate(std::size_t updatedPropertyIndex)` |
| `void` | `Setup(`[`Material`](documentation/generated/Graphics/Material.md)` const& material, `[`ShaderReflection`](documentation/generated/Graphics/ShaderReflection.md)` const& reflection)` |
| `void` | `Update(`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`& materialInstance)` |
| UniformValuePropertyHandler`&` | `operator=(`UniformValuePropertyHandler` const&)` |
| UniformValuePropertyHandler`&` | `operator=(`UniformValuePropertyHandler`&&)` |
