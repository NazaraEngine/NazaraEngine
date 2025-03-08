---
title: SkeletalMesh
description: Nothing
---

# Nz::SkeletalMesh

Class description

## Constructors

- `SkeletalMesh(std::shared_ptr<`[`VertexBuffer`](documentation/generated/Utility/VertexBuffer.md)`> vertexBuffer, std::shared_ptr<`[`IndexBuffer`](documentation/generated/Utility/IndexBuffer.md)`> indexBuffer)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::Boxf const&` | `GetAABB()` |
| `Nz::AnimationType` | `GetAnimationType()` |
| `std::shared_ptr<`[`IndexBuffer`](documentation/generated/Utility/IndexBuffer.md)`> const&` | `GetIndexBuffer()` |
| `std::shared_ptr<`[`VertexBuffer`](documentation/generated/Utility/VertexBuffer.md)`> const&` | `GetVertexBuffer()` |
| `Nz::UInt32` | `GetVertexCount()` |
| `bool` | `IsAnimated()` |
| `bool` | `IsValid()` |
| `void` | `SetAABB(Nz::Boxf const& aabb)` |
| `void` | `SetIndexBuffer(std::shared_ptr<`[`IndexBuffer`](documentation/generated/Utility/IndexBuffer.md)`> indexBuffer)` |
