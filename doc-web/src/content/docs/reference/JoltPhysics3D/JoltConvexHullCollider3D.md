---
title: JoltConvexHullCollider3D
description: Nothing
---

# Nz::JoltConvexHullCollider3D

Class description

## Constructors

- `JoltConvexHullCollider3D(SparsePtr<const Nz::Vector3f> vertices, std::size_t vertexCount, float hullTolerance, float convexRadius, float maxErrorConvexRadius)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, Nz::Matrix4f const& offsetMatrix)` |
| `Nz::JoltColliderType3D` | `GetType()` |
