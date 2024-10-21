---
title: TextSprite
description: Nothing
---

# Nz::TextSprite

Class description

## Constructors

- `TextSprite(std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> material)`
- `TextSprite(`TextSprite` const&)`
- `TextSprite(`TextSprite`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `BuildElement(`[`ElementRendererRegistry`](documentation/generated/Graphics/ElementRendererRegistry.md)`& registry, `[`InstancedRenderable::ElementData`](documentation/generated/Graphics/InstancedRenderable.ElementData.md)` const& elementData, std::size_t passIndex, std::vector<`[`RenderElementOwner`](documentation/generated/Graphics/RenderElementOwner.md)`>& elements)` |
| `void` | `Clear()` |
| `std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> const&` | `GetMaterial(std::size_t i)` |
| `std::size_t` | `GetMaterialCount()` |
| `void` | `SetMaterial(std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> material)` |
| `void` | `Update(`[`AbstractTextDrawer`](documentation/generated/Utility/AbstractTextDrawer.md)` const& drawer, float scale)` |
| TextSprite`&` | `operator=(`TextSprite` const&)` |
| TextSprite`&` | `operator=(`TextSprite`&&)` |
