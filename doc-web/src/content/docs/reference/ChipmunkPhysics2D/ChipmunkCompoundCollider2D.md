---
title: ChipmunkCompoundCollider2D
description: Nothing
---

# Nz::ChipmunkCompoundCollider2D

Class description

## Constructors

- `ChipmunkCompoundCollider2D(std::vector<std::shared_ptr<`[`ChipmunkCollider2D`](documentation/generated/ChipmunkPhysics2D/ChipmunkCollider2D.md)`>> geoms)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::Vector2f` | `ComputeCenterOfMass()` |
| `float` | `ComputeMomentOfInertia(float mass)` |
| `bool` | `DoesOverrideCollisionProperties()` |
| `std::vector<std::shared_ptr<`[`ChipmunkCollider2D`](documentation/generated/ChipmunkPhysics2D/ChipmunkCollider2D.md)`>> const&` | `GetGeoms()` |
| `Nz::ChipmunkColliderType2D` | `GetType()` |
| `void` | `OverridesCollisionProperties(bool shouldOverride)` |
