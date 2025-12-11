---
title: SkeletonInstance
description: Nothing
---

# Nz::SkeletonInstance

Class description

## Constructors

- `SkeletonInstance(std::shared_ptr<const `[`Skeleton`](documentation/generated/Utility/Skeleton.md)`> skeleton)`
- `SkeletonInstance(`SkeletonInstance` const&)`
- `SkeletonInstance(`SkeletonInstance`&& skeletonInstance)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::shared_ptr<`[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)`>&` | `GetSkeletalBuffer()` |
| `std::shared_ptr<`[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)`> const&` | `GetSkeletalBuffer()` |
| `std::shared_ptr<const `[`Skeleton`](documentation/generated/Utility/Skeleton.md)`> const&` | `GetSkeleton()` |
| `void` | `OnTransfer(`[`RenderFrame`](documentation/generated/Renderer/RenderFrame.md)`& renderFrame, `[`CommandBufferBuilder`](documentation/generated/Renderer/CommandBufferBuilder.md)`& builder)` |
| SkeletonInstance`&` | `operator=(`SkeletonInstance` const&)` |
| SkeletonInstance`&` | `operator=(`SkeletonInstance`&& skeletonInstance)` |
