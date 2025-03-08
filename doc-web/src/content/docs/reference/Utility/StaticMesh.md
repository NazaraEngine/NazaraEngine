---
title: StaticMesh
description: Nothing
---

# Nz::StaticMesh

Class description

## Constructors

- `StaticMesh(std::shared_ptr<`[`VertexBuffer`](documentation/generated/Utility/VertexBuffer.md)`> vertexBuffer, std::shared_ptr<`[`IndexBuffer`](documentation/generated/Utility/IndexBuffer.md)`> indexBuffer)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Center()` |
| `bool` | `GenerateAABB()` |
| `Nz::Boxf const&` | `GetAABB()` |
| `Nz::AnimationType` | `GetAnimationType()` |
| `std::shared_ptr<`[`IndexBuffer`](documentation/generated/Utility/IndexBuffer.md)`> const&` | `GetIndexBuffer()` |
| `std::shared_ptr<`[`VertexBuffer`](documentation/generated/Utility/VertexBuffer.md)`> const&` | `GetVertexBuffer()` |
| `Nz::UInt32` | `GetVertexCount()` |
| `bool` | `IsAnimated()` |
| `bool` | `IsValid()` |
| `void` | `SetAABB(Nz::Boxf const& aabb)` |
| `void` | `SetIndexBuffer(std::shared_ptr<`[`IndexBuffer`](documentation/generated/Utility/IndexBuffer.md)`> indexBuffer)` |
