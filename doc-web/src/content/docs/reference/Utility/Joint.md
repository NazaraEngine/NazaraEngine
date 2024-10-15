---
title: Joint
description: Nothing
---

# Nz::Joint

Class description

## Constructors

- `Joint(`[`Skeleton`](documentation/generated/Utility/Skeleton.md)`* skeleton)`
- `Joint(`Joint` const& joint)`
- `Joint(`Joint`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `EnsureSkinningMatrixUpdate()` |
| `Nz::Matrix4f const&` | `GetInverseBindMatrix()` |
| `std::string const&` | `GetName()` |
| [`Skeleton`](documentation/generated/Utility/Skeleton.md)`*` | `GetSkeleton()` |
| [`Skeleton`](documentation/generated/Utility/Skeleton.md)` const*` | `GetSkeleton()` |
| `Nz::Matrix4f const&` | `GetSkinningMatrix()` |
| `void` | `SetInverseBindMatrix(Nz::Matrix4f const& matrix)` |
| `void` | `SetName(std::string name)` |
| Joint`&` | `operator=(`Joint` const& joint)` |
| Joint`&` | `operator=(`Joint`&& joint)` |
