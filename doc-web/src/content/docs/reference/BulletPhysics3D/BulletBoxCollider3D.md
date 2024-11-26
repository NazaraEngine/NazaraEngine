---
title: BulletBoxCollider3D
description: Nothing
---

# Nz::BulletBoxCollider3D

Class description

## Constructors

- `BulletBoxCollider3D(Nz::Vector3f const& lengths)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, Nz::Matrix4f const& offsetMatrix)` |
| `Nz::Vector3f` | `GetLengths()` |
| `btCollisionShape*` | `GetShape()` |
| `Nz::BulletColliderType3D` | `GetType()` |
