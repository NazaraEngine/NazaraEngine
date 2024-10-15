---
title: Sprite
description: Nothing
---

# Nz::Sprite

Class description

## Constructors

- `Sprite(std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> material)`
- `Sprite(`Sprite` const&)`
- `Sprite(`Sprite`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `BuildElement(`[`ElementRendererRegistry`](documentation/generated/Graphics/ElementRendererRegistry.md)`& registry, `[`InstancedRenderable::ElementData`](documentation/generated/Graphics/InstancedRenderable.ElementData.md)` const& elementData, std::size_t passIndex, std::vector<`[`RenderElementOwner`](documentation/generated/Graphics/RenderElementOwner.md)`>& elements)` |
| [`Color`](documentation/generated/Core/Color.md)` const&` | `GetColor()` |
| [`Color`](documentation/generated/Core/Color.md)` const&` | `GetCornerColor(Nz::RectCorner corner)` |
| `std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> const&` | `GetMaterial(std::size_t i)` |
| `std::size_t` | `GetMaterialCount()` |
| `Nz::Vector2f const&` | `GetOrigin()` |
| `Nz::Vector2f const&` | `GetSize()` |
| `Nz::Rectf const&` | `GetTextureCoords()` |
| `Nz::Vector3ui` | `GetTextureSize()` |
| `void` | `SetColor(`[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `SetCornerColor(Nz::RectCorner corner, `[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `SetMaterial(std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> material)` |
| `void` | `SetOrigin(Nz::Vector2f const& origin)` |
| `void` | `SetSize(Nz::Vector2f const& size)` |
| `void` | `SetTextureCoords(Nz::Rectf const& textureCoords)` |
| `void` | `SetTextureRect(Nz::Rectf const& textureRect)` |
| Sprite`&` | `operator=(`Sprite` const&)` |
| Sprite`&` | `operator=(`Sprite`&&)` |
