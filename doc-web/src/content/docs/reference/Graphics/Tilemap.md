---
title: Tilemap
description: Nothing
---

# Nz::Tilemap

Class description

## Constructors

- `Tilemap(Nz::Vector2ui const& mapSize, Nz::Vector2f const& tileSize, std::size_t materialCount)`
- `Tilemap(`Tilemap` const&)`
- `Tilemap(`Tilemap`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `BuildElement(`[`ElementRendererRegistry`](documentation/generated/Graphics/ElementRendererRegistry.md)`& registry, `[`InstancedRenderable::ElementData`](documentation/generated/Graphics/InstancedRenderable.ElementData.md)` const& elementData, std::size_t passIndex, std::vector<`[`RenderElementOwner`](documentation/generated/Graphics/RenderElementOwner.md)`>& elements)` |
| `void` | `DisableTile(Nz::Vector2ui const& tilePos)` |
| `void` | `DisableTiles()` |
| `void` | `DisableTiles(Nz::Vector2ui const* tilesPos, std::size_t tileCount)` |
| `void` | `EnableIsometricMode(bool isometric)` |
| `void` | `EnableTile(Nz::Vector2ui const& tilePos, Nz::Rectf const& coords, `[`Color`](documentation/generated/Core/Color.md)` const& color, std::size_t materialIndex)` |
| `void` | `EnableTile(Nz::Vector2ui const& tilePos, Nz::Rectui const& rect, `[`Color`](documentation/generated/Core/Color.md)` const& color, std::size_t materialIndex)` |
| `void` | `EnableTiles(Nz::Rectf const& coords, `[`Color`](documentation/generated/Core/Color.md)` const& color, std::size_t materialIndex)` |
| `void` | `EnableTiles(Nz::Rectui const& rect, `[`Color`](documentation/generated/Core/Color.md)` const& color, std::size_t materialIndex)` |
| `void` | `EnableTiles(Nz::Vector2ui const* tilesPos, std::size_t tileCount, Nz::Rectf const& coords, `[`Color`](documentation/generated/Core/Color.md)` const& color, std::size_t materialIndex)` |
| `void` | `EnableTiles(Nz::Vector2ui const* tilesPos, std::size_t tileCount, Nz::Rectui const& rect, `[`Color`](documentation/generated/Core/Color.md)` const& color, std::size_t materialIndex)` |
| `Nz::Vector2ui const&` | `GetMapSize()` |
| `std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> const&` | `GetMaterial(std::size_t i)` |
| `std::size_t` | `GetMaterialCount()` |
| `Nz::Vector2f const&` | `GetOrigin()` |
| `Nz::Vector2f` | `GetSize()` |
| [`Tilemap::Tile`](documentation/generated/Graphics/Tilemap.Tile.md)` const&` | `GetTile(Nz::Vector2ui const& tilePos)` |
| `Nz::Vector2f const&` | `GetTileSize()` |
| `bool` | `IsIsometricModeEnabled()` |
| `void` | `SetMaterial(std::size_t matIndex, std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> material)` |
| `void` | `SetOrigin(Nz::Vector2f const& origin)` |
| Tilemap`&` | `operator=(`Tilemap` const&)` |
| Tilemap`&` | `operator=(`Tilemap`&&)` |
