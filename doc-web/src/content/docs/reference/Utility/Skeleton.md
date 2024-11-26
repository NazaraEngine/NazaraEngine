---
title: Skeleton
description: Nothing
---

# Nz::Skeleton

Class description

## Constructors

- `Skeleton()`
- `Skeleton(`Skeleton` const& skeleton)`
- `Skeleton(`Skeleton`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Create(std::size_t jointCount)` |
| `void` | `Destroy()` |
| `Nz::Boxf const&` | `GetAABB()` |
| [`Joint`](documentation/generated/Utility/Joint.md)`*` | `GetJoint(std::string const& jointName)` |
| [`Joint`](documentation/generated/Utility/Joint.md)`*` | `GetJoint(std::size_t index)` |
| [`Joint`](documentation/generated/Utility/Joint.md)` const*` | `GetJoint(std::string const& jointName)` |
| [`Joint`](documentation/generated/Utility/Joint.md)` const*` | `GetJoint(std::size_t index)` |
| [`Joint`](documentation/generated/Utility/Joint.md)`*` | `GetJoints()` |
| [`Joint`](documentation/generated/Utility/Joint.md)` const*` | `GetJoints()` |
| `std::size_t` | `GetJointCount()` |
| `std::size_t` | `GetJointIndex(std::string const& jointName)` |
| [`Joint`](documentation/generated/Utility/Joint.md)`*` | `GetRootJoint()` |
| [`Joint`](documentation/generated/Utility/Joint.md)` const*` | `GetRootJoint()` |
| `void` | `Interpolate(`Skeleton` const& skeletonA, `Skeleton` const& skeletonB, float interpolation)` |
| `void` | `Interpolate(`Skeleton` const& skeletonA, `Skeleton` const& skeletonB, float interpolation, std::size_t const* indices, std::size_t indiceCount)` |
| `bool` | `IsValid()` |
| Skeleton`&` | `operator=(`Skeleton` const& skeleton)` |
| Skeleton`&` | `operator=(`Skeleton`&&)` |
