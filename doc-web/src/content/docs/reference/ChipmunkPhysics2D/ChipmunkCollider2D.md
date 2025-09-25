---
title: ChipmunkCollider2D
description: Nothing
---

# Nz::ChipmunkCollider2D

Class description

## Constructors

- `ChipmunkCollider2D()`
- `ChipmunkCollider2D(`ChipmunkCollider2D` const&)`
- `ChipmunkCollider2D(`ChipmunkCollider2D`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::Vector2f` | `ComputeCenterOfMass()` |
| `float` | `ComputeMomentOfInertia(float mass)` |
| `void` | `ForEachPolygon(std::function<void (const Vector2f *, std::size_t)> const& callback)` |
| `Nz::UInt32` | `GetCategoryMask()` |
| `Nz::UInt32` | `GetCollisionGroup()` |
| `unsigned int` | `GetCollisionId()` |
| `Nz::UInt32` | `GetCollisionMask()` |
| `float` | `GetElasticity()` |
| `float` | `GetFriction()` |
| `Nz::Vector2f` | `GetSurfaceVelocity()` |
| `Nz::ChipmunkColliderType2D` | `GetType()` |
| `bool` | `IsTrigger()` |
| `void` | `SetCategoryMask(Nz::UInt32 categoryMask)` |
| `void` | `SetCollisionGroup(Nz::UInt32 groupId)` |
| `void` | `SetCollisionId(unsigned int typeId)` |
| `void` | `SetCollisionMask(Nz::UInt32 mask)` |
| `void` | `SetElasticity(float elasticity)` |
| `void` | `SetFriction(float friction)` |
| `void` | `SetSurfaceVelocity(Nz::Vector2f const& surfaceVelocity)` |
| `void` | `SetTrigger(bool trigger)` |
| ChipmunkCollider2D`&` | `operator=(`ChipmunkCollider2D` const&)` |
| ChipmunkCollider2D`&` | `operator=(`ChipmunkCollider2D`&&)` |
