---
title: BulletCompoundCollider3D
description: Nothing
---

# Nz::BulletCompoundCollider3D

Class description

## Constructors

- `BulletCompoundCollider3D(std::vector<ChildCollider> childs)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, Nz::Matrix4f const& offsetMatrix)` |
| `std::vector<ChildCollider> const&` | `GetGeoms()` |
| `btCollisionShape*` | `GetShape()` |
| `Nz::BulletColliderType3D` | `GetType()` |
