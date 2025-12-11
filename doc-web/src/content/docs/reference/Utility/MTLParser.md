---
title: MTLParser
description: Nothing
---

# Nz::MTLParser

Class description

## Constructors

- `MTLParser()`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`MTLParser::Material`](documentation/generated/Utility/MTLParser.Material.md)`*` | `AddMaterial(std::string const& matName)` |
| `void` | `Clear()` |
| [`MTLParser::Material`](documentation/generated/Utility/MTLParser.Material.md)` const*` | `GetMaterial(std::string const& materialName)` |
| `std::unordered_map<std::string, `[`Material`](documentation/generated/Graphics/Material.md)`> const&` | `GetMaterials()` |
| `bool` | `Parse(`[`Stream`](documentation/generated/Core/Stream.md)`& stream)` |
| `bool` | `Save(`[`Stream`](documentation/generated/Core/Stream.md)`& stream)` |
