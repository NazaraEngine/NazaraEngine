---
title: JoltPhysWorld3D
description: Nothing
---

# Nz::JoltPhysWorld3D

Class description

## Constructors

- `JoltPhysWorld3D()`
- `JoltPhysWorld3D(`JoltPhysWorld3D` const&)`
- `JoltPhysWorld3D(`JoltPhysWorld3D`&& ph)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::UInt32` | `GetActiveBodyCount()` |
| `Nz::Vector3f` | `GetGravity()` |
| `std::size_t` | `GetMaxStepCount()` |
| `JPH::PhysicsSystem*` | `GetPhysicsSystem()` |
| `Nz::Time` | `GetStepSize()` |
| `bool` | `IsBodyActive(Nz::UInt32 bodyIndex)` |
| `bool` | `IsBodyRegistered(Nz::UInt32 bodyIndex)` |
| `bool` | `RaycastQuery(Nz::Vector3f const& from, Nz::Vector3f const& to, FunctionRef<std::optional<float> (const `[`JoltPhysWorld3D::RaycastHit`](documentation/generated/JoltPhysics3D/JoltPhysWorld3D.RaycastHit.md)` &)> const& callback)` |
| `bool` | `RaycastQueryFirst(Nz::Vector3f const& from, Nz::Vector3f const& to, FunctionRef<void (const `[`JoltPhysWorld3D::RaycastHit`](documentation/generated/JoltPhysics3D/JoltPhysWorld3D.RaycastHit.md)` &)> const& callback)` |
| `void` | `RefreshBodies()` |
| `void` | `RegisterStepListener(`[`JoltPhysicsStepListener`](documentation/generated/JoltPhysics3D/JoltPhysicsStepListener.md)`* character)` |
| `void` | `SetGravity(Nz::Vector3f const& gravity)` |
| `void` | `SetMaxStepCount(std::size_t maxStepCount)` |
| `void` | `SetStepSize(`[`Time`](documentation/generated/Core/Time.md)` stepSize)` |
| `void` | `Step(`[`Time`](documentation/generated/Core/Time.md)` timestep)` |
| `void` | `UnregisterStepListener(`[`JoltPhysicsStepListener`](documentation/generated/JoltPhysics3D/JoltPhysicsStepListener.md)`* character)` |
| JoltPhysWorld3D`&` | `operator=(`JoltPhysWorld3D` const&)` |
| JoltPhysWorld3D`&` | `operator=(`JoltPhysWorld3D`&&)` |
