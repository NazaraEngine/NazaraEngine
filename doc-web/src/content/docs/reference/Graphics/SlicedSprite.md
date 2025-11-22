---
title: SlicedSprite
description: Nothing
---

# Nz::SlicedSprite

Class description

## Constructors

- `SlicedSprite(std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> material)`
- `SlicedSprite(`SlicedSprite` const&)`
- `SlicedSprite(`SlicedSprite`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `BuildElement(`[`ElementRendererRegistry`](documentation/generated/Graphics/ElementRendererRegistry.md)`& registry, `[`InstancedRenderable::ElementData`](documentation/generated/Graphics/InstancedRenderable.ElementData.md)` const& elementData, std::size_t passIndex, std::vector<`[`RenderElementOwner`](documentation/generated/Graphics/RenderElementOwner.md)`>& elements)` |
| [`Color`](documentation/generated/Core/Color.md)` const&` | `GetColor()` |
| [`SlicedSprite::Corner`](documentation/generated/Graphics/SlicedSprite.Corner.md)` const&` | `GetBottomRightCorner()` |
| `std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> const&` | `GetMaterial(std::size_t i)` |
| `std::size_t` | `GetMaterialCount()` |
| `Nz::Vector2f const&` | `GetOrigin()` |
| `Nz::Vector2f const&` | `GetSize()` |
| [`SlicedSprite::Corner`](documentation/generated/Graphics/SlicedSprite.Corner.md)` const&` | `GetTopLeftCorner()` |
| `Nz::Rectf const&` | `GetTextureCoords()` |
| `Nz::Vector3ui` | `GetTextureSize()` |
| `void` | `SetColor(`[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `SetCorners(`[`SlicedSprite::Corner`](documentation/generated/Graphics/SlicedSprite.Corner.md)` const& topLeftCorner, `[`SlicedSprite::Corner`](documentation/generated/Graphics/SlicedSprite.Corner.md)` const& bottomRightCorner)` |
| `void` | `SetCornersSize(Nz::Vector2f const& topLeftSize, Nz::Vector2f const& bottomRightSize)` |
| `void` | `SetCornersTextureCoords(Nz::Vector2f const& topLeftTextureCoords, Nz::Vector2f const& bottomRightTextureCoords)` |
| `void` | `SetMaterial(std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> material)` |
| `void` | `SetOrigin(Nz::Vector2f const& origin)` |
| `void` | `SetSize(Nz::Vector2f const& size)` |
| `void` | `SetTextureCoords(Nz::Rectf const& textureCoords)` |
| `void` | `SetTextureRect(Nz::Rectf const& textureRect)` |
| SlicedSprite`&` | `operator=(`SlicedSprite` const&)` |
| SlicedSprite`&` | `operator=(`SlicedSprite`&&)` |
