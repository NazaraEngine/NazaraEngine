---
title: SubMesh
description: Nothing
---

# Nz::SubMesh

Class description

## Constructors

- `SubMesh()`
- `SubMesh(`SubMesh` const&)`
- `SubMesh(`SubMesh`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `GenerateNormals()` |
| `void` | `GenerateNormalsAndTangents()` |
| `void` | `GenerateTangents()` |
| `Nz::Boxf const&` | `GetAABB()` |
| `Nz::AnimationType` | `GetAnimationType()` |
| `std::shared_ptr<`[`IndexBuffer`](documentation/generated/Utility/IndexBuffer.md)`> const&` | `GetIndexBuffer()` |
| `std::size_t` | `GetMaterialIndex()` |
| `Nz::PrimitiveMode` | `GetPrimitiveMode()` |
| `Nz::UInt32` | `GetTriangleCount()` |
| `Nz::UInt32` | `GetVertexCount()` |
| `bool` | `IsAnimated()` |
| `void` | `SetMaterialIndex(std::size_t matIndex)` |
| `void` | `SetPrimitiveMode(Nz::PrimitiveMode mode)` |
| SubMesh`&` | `operator=(`SubMesh` const&)` |
| SubMesh`&` | `operator=(`SubMesh`&&)` |
