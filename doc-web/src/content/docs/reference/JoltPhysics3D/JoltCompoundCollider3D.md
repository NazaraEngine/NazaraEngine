---
title: JoltCompoundCollider3D
description: Nothing
---

# Nz::JoltCompoundCollider3D

Class description

## Constructors

- `JoltCompoundCollider3D(std::vector<ChildCollider> childs)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, Nz::Matrix4f const& offsetMatrix)` |
| `std::vector<ChildCollider> const&` | `GetGeoms()` |
| `Nz::JoltColliderType3D` | `GetType()` |
