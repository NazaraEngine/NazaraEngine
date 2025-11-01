---
title: JoltCollider3D
description: Nothing
---

# Nz::JoltCollider3D

Class description

## Constructors

- `JoltCollider3D()`
- `JoltCollider3D(`JoltCollider3D` const&)`
- `JoltCollider3D(`JoltCollider3D`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, Nz::Matrix4f const& offsetMatrix)` |
| `std::shared_ptr<`[`StaticMesh`](documentation/generated/Utility/StaticMesh.md)`>` | `GenerateDebugMesh()` |
| `JPH::ShapeSettings*` | `GetShapeSettings()` |
| `Nz::JoltColliderType3D` | `GetType()` |
| JoltCollider3D`&` | `operator=(`JoltCollider3D` const&)` |
| JoltCollider3D`&` | `operator=(`JoltCollider3D`&&)` |
