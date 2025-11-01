---
title: JoltMeshCollider3D
description: Nothing
---

# Nz::JoltMeshCollider3D

Class description

## Constructors

- `JoltMeshCollider3D(SparsePtr<const Nz::Vector3f> vertices, std::size_t vertexCount, SparsePtr<const Nz::UInt16> indices, std::size_t indexCount)`
- `JoltMeshCollider3D(SparsePtr<const Nz::Vector3f> vertices, std::size_t vertexCount, SparsePtr<const Nz::UInt32> indices, std::size_t indexCount)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, Nz::Matrix4f const& offsetMatrix)` |
| `Nz::JoltColliderType3D` | `GetType()` |
