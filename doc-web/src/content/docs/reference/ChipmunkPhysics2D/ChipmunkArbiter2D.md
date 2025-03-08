---
title: ChipmunkArbiter2D
description: Nothing
---

# Nz::ChipmunkArbiter2D

Class description

## Constructors

- `ChipmunkArbiter2D(cpArbiter* arbiter)`
- `ChipmunkArbiter2D(`ChipmunkArbiter2D` const&)`
- `ChipmunkArbiter2D(`ChipmunkArbiter2D`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `float` | `ComputeTotalKinematicEnergy()` |
| `Nz::Vector2f` | `ComputeTotalImpulse()` |
| `std::pair<`[`ChipmunkRigidBody2D`](documentation/generated/ChipmunkPhysics2D/ChipmunkRigidBody2D.md)` *, `[`ChipmunkRigidBody2D`](documentation/generated/ChipmunkPhysics2D/ChipmunkRigidBody2D.md)` *>` | `GetBodies()` |
| `std::size_t` | `GetContactCount()` |
| `float` | `GetContactDepth(std::size_t i)` |
| `Nz::Vector2f` | `GetContactPointA(std::size_t i)` |
| `Nz::Vector2f` | `GetContactPointB(std::size_t i)` |
| `float` | `GetElasticity()` |
| `float` | `GetFriction()` |
| `Nz::Vector2f` | `GetNormal()` |
| `Nz::Vector2f` | `GetSurfaceVelocity()` |
| `bool` | `IsFirstContact()` |
| `bool` | `IsRemoval()` |
| `void` | `SetElasticity(float elasticity)` |
| `void` | `SetFriction(float friction)` |
| `void` | `SetSurfaceVelocity(Nz::Vector2f const& surfaceVelocity)` |
| ChipmunkArbiter2D`&` | `operator=(`ChipmunkArbiter2D` const&)` |
| ChipmunkArbiter2D`&` | `operator=(`ChipmunkArbiter2D`&&)` |
