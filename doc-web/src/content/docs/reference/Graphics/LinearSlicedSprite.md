---
title: LinearSlicedSprite
description: Nothing
---

# Nz::LinearSlicedSprite

Class description

## Constructors

- `LinearSlicedSprite(std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> material, Nz::LinearSlicedSprite::Orientation orientation)`
- `LinearSlicedSprite(`LinearSlicedSprite` const&)`
- `LinearSlicedSprite(`LinearSlicedSprite`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `AddSection(float size, float textureCoord)` |
| `void` | `BuildElement(`[`ElementRendererRegistry`](documentation/generated/Graphics/ElementRendererRegistry.md)`& registry, `[`InstancedRenderable::ElementData`](documentation/generated/Graphics/InstancedRenderable.ElementData.md)` const& elementData, std::size_t passIndex, std::vector<`[`RenderElementOwner`](documentation/generated/Graphics/RenderElementOwner.md)`>& elements)` |
| `void` | `Clear()` |
| [`Color`](documentation/generated/Core/Color.md)` const&` | `GetColor()` |
| `std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> const&` | `GetMaterial(std::size_t i)` |
| `std::size_t` | `GetMaterialCount()` |
| `Nz::LinearSlicedSprite::Orientation` | `GetOrientation()` |
| `Nz::Vector2f const&` | `GetOrigin()` |
| [`LinearSlicedSprite::Section`](documentation/generated/Graphics/LinearSlicedSprite.Section.md)` const&` | `GetSection(std::size_t sectionIndex)` |
| `std::size_t` | `GetSectionCount()` |
| `float` | `GetSize()` |
| `Nz::Rectf const&` | `GetTextureCoords()` |
| `Nz::Vector3ui` | `GetTextureSize()` |
| `void` | `RemoveSection(std::size_t sectionIndex)` |
| `void` | `SetColor(`[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `SetMaterial(std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> material)` |
| `void` | `SetOrigin(Nz::Vector2f const& origin)` |
| `void` | `SetSection(std::size_t sectionIndex, float size, float textureCoord)` |
| `void` | `SetSectionSize(std::size_t sectionIndex, float size)` |
| `void` | `SetSectionTextureCoord(std::size_t sectionIndex, float textureCoord)` |
| `void` | `SetSize(float size)` |
| `void` | `SetTextureCoords(Nz::Rectf const& textureCoords)` |
| `void` | `SetTextureRect(Nz::Rectf const& textureRect)` |
| LinearSlicedSprite`&` | `operator=(`LinearSlicedSprite` const&)` |
| LinearSlicedSprite`&` | `operator=(`LinearSlicedSprite`&&)` |
