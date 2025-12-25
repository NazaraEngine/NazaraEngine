---
title: SkeletonComponentBase
description: Nothing
---

# Nz::SkeletonComponentBase

Class description

## Constructors

- `SkeletonComponentBase(`SkeletonComponentBase` const&)`
- `SkeletonComponentBase(`SkeletonComponentBase`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::size_t` | `FindJointByName(std::string const& jointName)` |
| [`Joint`](documentation/generated/Utility/Joint.md)` const&` | `GetAttachedJoint(std::size_t jointIndex)` |
| `std::shared_ptr<`[`Skeleton`](documentation/generated/Utility/Skeleton.md)`> const&` | `GetSkeleton()` |
| SkeletonComponentBase`&` | `operator=(`SkeletonComponentBase` const&)` |
| SkeletonComponentBase`&` | `operator=(`SkeletonComponentBase`&&)` |
