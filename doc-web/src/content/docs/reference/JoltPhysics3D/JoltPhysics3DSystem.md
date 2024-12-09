---
title: JoltPhysics3DSystem
description: Nothing
---

# Nz::JoltPhysics3DSystem

Class description

## Constructors

- `JoltPhysics3DSystem(entt::registry& registry)`
- `JoltPhysics3DSystem(`JoltPhysics3DSystem` const&)`
- `JoltPhysics3DSystem(`JoltPhysics3DSystem`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Dump()` |
| [`JoltPhysWorld3D`](documentation/generated/JoltPhysics3D/JoltPhysWorld3D.md)`&` | `GetPhysWorld()` |
| [`JoltPhysWorld3D`](documentation/generated/JoltPhysics3D/JoltPhysWorld3D.md)` const&` | `GetPhysWorld()` |
| `bool` | `RaycastQuery(Nz::Vector3f const& from, Nz::Vector3f const& to, FunctionRef<std::optional<float> (const `[`JoltPhysics3DSystem::RaycastHit`](documentation/generated/JoltPhysics3D/JoltPhysics3DSystem.RaycastHit.md)` &)> const& callback)` |
| `bool` | `RaycastQueryFirst(Nz::Vector3f const& from, Nz::Vector3f const& to, FunctionRef<void (const `[`JoltPhysics3DSystem::RaycastHit`](documentation/generated/JoltPhysics3D/JoltPhysics3DSystem.RaycastHit.md)` &)> const& callback)` |
| `void` | `Update(`[`Time`](documentation/generated/Core/Time.md)` elapsedTime)` |
| JoltPhysics3DSystem`&` | `operator=(`JoltPhysics3DSystem` const&)` |
| JoltPhysics3DSystem`&` | `operator=(`JoltPhysics3DSystem`&&)` |
