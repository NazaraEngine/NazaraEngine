---
title: BulletCollider3D
description: Nothing
---

# Nz::BulletCollider3D

Class description

## Constructors

- `BulletCollider3D()`
- `BulletCollider3D(`BulletCollider3D` const&)`
- `BulletCollider3D(`BulletCollider3D`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, Nz::Matrix4f const& offsetMatrix)` |
| `Nz::Boxf` | `ComputeAABB(Nz::Vector3f const& translation, Nz::Quaternionf const& rotation, Nz::Vector3f const& scale)` |
| `Nz::Boxf` | `ComputeAABB(Nz::Matrix4f const& offsetMatrix, Nz::Vector3f const& scale)` |
| `void` | `ComputeInertia(float mass, Nz::Vector3f* inertia)` |
| `std::shared_ptr<`[`StaticMesh`](documentation/generated/Utility/StaticMesh.md)`>` | `GenerateDebugMesh()` |
| `btCollisionShape*` | `GetShape()` |
| `Nz::BulletColliderType3D` | `GetType()` |
| BulletCollider3D`&` | `operator=(`BulletCollider3D` const&)` |
| BulletCollider3D`&` | `operator=(`BulletCollider3D`&&)` |
