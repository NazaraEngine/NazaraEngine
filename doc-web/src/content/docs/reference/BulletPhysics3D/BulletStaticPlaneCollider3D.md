---
title: BulletStaticPlaneCollider3D
description: Nothing
---

# Nz::BulletStaticPlaneCollider3D

Class description

## Constructors

- `BulletStaticPlaneCollider3D(Nz::Planef const& plane)`
- `BulletStaticPlaneCollider3D(Nz::Vector3f const& normal, float distance)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, Nz::Matrix4f const& offsetMatrix)` |
| `float` | `GetDistance()` |
| `Nz::Vector3f const&` | `GetNormal()` |
| `btCollisionShape*` | `GetShape()` |
| `Nz::BulletColliderType3D` | `GetType()` |
