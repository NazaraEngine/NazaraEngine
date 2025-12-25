---
title: Animation
description: Nothing
---

# Nz::Animation

Class description

## Constructors

- `Animation()`
- `Animation(`Animation` const&)`
- `Animation(`Animation`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `AddSequence(`[`Sequence`](documentation/generated/Utility/Sequence.md)` const& sequence)` |
| `void` | `AnimateSkeleton(`[`Skeleton`](documentation/generated/Utility/Skeleton.md)`* targetSkeleton, std::size_t frameA, std::size_t frameB, float interpolation)` |
| `bool` | `CreateSkeletal(std::size_t frameCount, std::size_t jointCount)` |
| `void` | `Destroy()` |
| `void` | `EnableLoopPointInterpolation(bool loopPointInterpolation)` |
| `std::size_t` | `GetFrameCount()` |
| `std::size_t` | `GetJointCount()` |
| [`Sequence`](documentation/generated/Utility/Sequence.md)`*` | `GetSequence(std::string const& sequenceName)` |
| [`Sequence`](documentation/generated/Utility/Sequence.md)`*` | `GetSequence(std::size_t index)` |
| [`Sequence`](documentation/generated/Utility/Sequence.md)` const*` | `GetSequence(std::string const& sequenceName)` |
| [`Sequence`](documentation/generated/Utility/Sequence.md)` const*` | `GetSequence(std::size_t index)` |
| `std::size_t` | `GetSequenceCount()` |
| `std::size_t` | `GetSequenceIndex(std::string const& sequenceName)` |
| [`SequenceJoint`](documentation/generated/Utility/SequenceJoint.md)`*` | `GetSequenceJoints(std::size_t frameIndex)` |
| [`SequenceJoint`](documentation/generated/Utility/SequenceJoint.md)` const*` | `GetSequenceJoints(std::size_t frameIndex)` |
| `Nz::AnimationType` | `GetType()` |
| `bool` | `HasSequence(std::string const& sequenceName)` |
| `bool` | `HasSequence(std::size_t index)` |
| `bool` | `IsLoopPointInterpolationEnabled()` |
| `bool` | `IsValid()` |
| `void` | `RemoveSequence(std::string const& sequenceName)` |
| `void` | `RemoveSequence(std::size_t index)` |
| Animation`&` | `operator=(`Animation` const&)` |
| Animation`&` | `operator=(`Animation`&&)` |
