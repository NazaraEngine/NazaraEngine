---
title: Model
description: Nothing
---

# Nz::Model

Class description

## Constructors

- `Model(std::shared_ptr<`[`GraphicalMesh`](documentation/generated/Graphics/GraphicalMesh.md)`> graphicalMesh)`
- `Model(`Model` const&)`
- `Model(`Model`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `BuildElement(`[`ElementRendererRegistry`](documentation/generated/Graphics/ElementRendererRegistry.md)`& registry, `[`InstancedRenderable::ElementData`](documentation/generated/Graphics/InstancedRenderable.ElementData.md)` const& elementData, std::size_t passIndex, std::vector<`[`RenderElementOwner`](documentation/generated/Graphics/RenderElementOwner.md)`>& elements)` |
| `std::shared_ptr<`[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)`> const&` | `GetIndexBuffer(std::size_t subMeshIndex)` |
| `std::size_t` | `GetIndexCount(std::size_t subMeshIndex)` |
| `std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> const&` | `GetMaterial(std::size_t subMeshIndex)` |
| `std::size_t` | `GetMaterialCount()` |
| `std::size_t` | `GetSubMeshCount()` |
| `std::vector<`[`RenderPipelineInfo::VertexBufferData`](documentation/generated/Renderer/RenderPipelineInfo.VertexBufferData.md)`> const&` | `GetVertexBufferData(std::size_t subMeshIndex)` |
| `std::shared_ptr<`[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)`> const&` | `GetVertexBuffer(std::size_t subMeshIndex)` |
| `void` | `SetMaterial(std::size_t subMeshIndex, std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> material)` |
| Model`&` | `operator=(`Model` const&)` |
| Model`&` | `operator=(`Model`&&)` |
