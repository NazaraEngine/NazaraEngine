---
title: BulletPivotConstraint3D
description: Nothing
---

# Nz::BulletPivotConstraint3D

Class description

## Constructors

- `BulletPivotConstraint3D(`[`BulletRigidBody3D`](documentation/generated/BulletPhysics3D/BulletRigidBody3D.md)`& first, Nz::Vector3f const& pivot)`
- `BulletPivotConstraint3D(`[`BulletRigidBody3D`](documentation/generated/BulletPhysics3D/BulletRigidBody3D.md)`& first, `[`BulletRigidBody3D`](documentation/generated/BulletPhysics3D/BulletRigidBody3D.md)`& second, Nz::Vector3f const& pivot, bool disableCollisions)`
- `BulletPivotConstraint3D(`[`BulletRigidBody3D`](documentation/generated/BulletPhysics3D/BulletRigidBody3D.md)`& first, `[`BulletRigidBody3D`](documentation/generated/BulletPhysics3D/BulletRigidBody3D.md)`& second, Nz::Vector3f const& firstAnchor, Nz::Vector3f const& secondAnchor, bool disableCollisions)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::Vector3f` | `GetFirstAnchor()` |
| `Nz::Vector3f` | `GetSecondAnchor()` |
| `float` | `GetDamping()` |
| `float` | `GetImpulseClamp()` |
| `void` | `SetFirstAnchor(Nz::Vector3f const& firstAnchor)` |
| `void` | `SetSecondAnchor(Nz::Vector3f const& secondAnchor)` |
| `void` | `SetDamping(float damping)` |
| `void` | `SetImpulseClamp(float impulseClamp)` |
