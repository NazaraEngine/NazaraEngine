---
title: BulletPhysWorld3D
description: Nothing
---

# Nz::BulletPhysWorld3D

Class description

## Constructors

- `BulletPhysWorld3D()`
- `BulletPhysWorld3D(`BulletPhysWorld3D` const&)`
- `BulletPhysWorld3D(`BulletPhysWorld3D`&& ph)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `btDynamicsWorld*` | `GetDynamicsWorld()` |
| `Nz::Vector3f` | `GetGravity()` |
| `std::size_t` | `GetMaxStepCount()` |
| `Nz::Time` | `GetStepSize()` |
| `bool` | `RaycastQuery(Nz::Vector3f const& from, Nz::Vector3f const& to, FunctionRef<std::optional<float> (const `[`BulletPhysWorld3D::RaycastHit`](documentation/generated/BulletPhysics3D/BulletPhysWorld3D.RaycastHit.md)` &)> const& callback)` |
| `bool` | `RaycastQueryFirst(Nz::Vector3f const& from, Nz::Vector3f const& to, `[`BulletPhysWorld3D::RaycastHit`](documentation/generated/BulletPhysics3D/BulletPhysWorld3D.RaycastHit.md)`* hitInfo)` |
| `void` | `SetGravity(Nz::Vector3f const& gravity)` |
| `void` | `SetMaxStepCount(std::size_t maxStepCount)` |
| `void` | `SetStepSize(`[`Time`](documentation/generated/Core/Time.md)` stepSize)` |
| `void` | `Step(`[`Time`](documentation/generated/Core/Time.md)` timestep)` |
| BulletPhysWorld3D`&` | `operator=(`BulletPhysWorld3D` const&)` |
| BulletPhysWorld3D`&` | `operator=(`BulletPhysWorld3D`&&)` |
