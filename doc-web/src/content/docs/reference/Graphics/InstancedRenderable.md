---
title: InstancedRenderable
description: Nothing
---

# Nz::InstancedRenderable

Class description

## Constructors

- `InstancedRenderable()`
- `InstancedRenderable(`InstancedRenderable` const&)`
- `InstancedRenderable(`InstancedRenderable`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `BuildElement(`[`ElementRendererRegistry`](documentation/generated/Graphics/ElementRendererRegistry.md)`& registry, `[`InstancedRenderable::ElementData`](documentation/generated/Graphics/InstancedRenderable.ElementData.md)` const& elementData, std::size_t passIndex, std::vector<`[`RenderElementOwner`](documentation/generated/Graphics/RenderElementOwner.md)`>& elements)` |
| `Nz::Boxf const&` | `GetAABB()` |
| `std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> const&` | `GetMaterial(std::size_t i)` |
| `std::size_t` | `GetMaterialCount()` |
| `int` | `GetRenderLayer()` |
| `void` | `UpdateRenderLayer(int renderLayer)` |
| InstancedRenderable`&` | `operator=(`InstancedRenderable` const&)` |
| InstancedRenderable`&` | `operator=(`InstancedRenderable`&&)` |
