---
title: JoltDistanceConstraint3D
description: Nothing
---

# Nz::JoltDistanceConstraint3D

Class description

## Constructors

- `JoltDistanceConstraint3D(`[`JoltRigidBody3D`](documentation/generated/JoltPhysics3D/JoltRigidBody3D.md)`& first, Nz::Vector3f const& pivot, float maxDist, float minDist)`
- `JoltDistanceConstraint3D(`[`JoltRigidBody3D`](documentation/generated/JoltPhysics3D/JoltRigidBody3D.md)`& first, `[`JoltRigidBody3D`](documentation/generated/JoltPhysics3D/JoltRigidBody3D.md)`& second, Nz::Vector3f const& pivot, float maxDist, float minDist)`
- `JoltDistanceConstraint3D(`[`JoltRigidBody3D`](documentation/generated/JoltPhysics3D/JoltRigidBody3D.md)`& first, `[`JoltRigidBody3D`](documentation/generated/JoltPhysics3D/JoltRigidBody3D.md)`& second, Nz::Vector3f const& firstAnchor, Nz::Vector3f const& secondAnchor, float maxDist, float minDist)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `float` | `GetDamping()` |
| `float` | `GetFrequency()` |
| `float` | `GetMaxDistance()` |
| `float` | `GetMinDistance()` |
| `void` | `SetDamping(float damping)` |
| `void` | `SetDistance(float minDist, float maxDist)` |
| `void` | `SetFrequency(float frequency)` |
| `void` | `SetMaxDistance(float maxDist)` |
| `void` | `SetMinDistance(float minDist)` |
