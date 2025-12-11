---
title: GraphicalMesh
description: Nothing
---

# Nz::GraphicalMesh

Class description

## Constructors

- `GraphicalMesh()`
- `GraphicalMesh(`GraphicalMesh` const&)`
- `GraphicalMesh(`GraphicalMesh`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::size_t` | `AddSubMesh(`[`GraphicalMesh::SubMesh`](documentation/generated/Graphics/GraphicalMesh.SubMesh.md)` subMesh)` |
| `void` | `Clear()` |
| `Nz::Boxf const&` | `GetAABB()` |
| `std::shared_ptr<`[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)`> const&` | `GetIndexBuffer(std::size_t subMesh)` |
| `Nz::UInt32` | `GetIndexCount(std::size_t subMesh)` |
| `Nz::IndexType` | `GetIndexType(std::size_t subMesh)` |
| `std::shared_ptr<`[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)`> const&` | `GetVertexBuffer(std::size_t subMesh)` |
| `std::shared_ptr<const `[`VertexDeclaration`](documentation/generated/Utility/VertexDeclaration.md)`> const&` | `GetVertexDeclaration(std::size_t subMesh)` |
| `std::size_t` | `GetSubMeshCount()` |
| `void` | `UpdateAABB(Nz::Boxf const& aabb)` |
| `void` | `UpdateSubMeshIndexCount(std::size_t subMeshIndex, Nz::UInt32 indexCount)` |
| GraphicalMesh`&` | `operator=(`GraphicalMesh` const&)` |
| GraphicalMesh`&` | `operator=(`GraphicalMesh`&&)` |
