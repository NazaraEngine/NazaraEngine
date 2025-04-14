---
title: BulletConvexCollider3D
description: Nothing
---

# Nz::BulletConvexCollider3D

Class description

## Constructors

- `BulletConvexCollider3D(SparsePtr<const Nz::Vector3f> vertices, unsigned int vertexCount)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, Nz::Matrix4f const& offsetMatrix)` |
| `btCollisionShape*` | `GetShape()` |
| `Nz::BulletColliderType3D` | `GetType()` |
