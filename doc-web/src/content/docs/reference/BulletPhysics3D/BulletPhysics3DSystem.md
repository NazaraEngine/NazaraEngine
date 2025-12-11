---
title: BulletPhysics3DSystem
description: Nothing
---

# Nz::BulletPhysics3DSystem

Class description

## Constructors

- `BulletPhysics3DSystem(entt::registry& registry)`
- `BulletPhysics3DSystem(`BulletPhysics3DSystem` const&)`
- `BulletPhysics3DSystem(`BulletPhysics3DSystem`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Dump()` |
| [`BulletPhysWorld3D`](documentation/generated/BulletPhysics3D/BulletPhysWorld3D.md)`&` | `GetPhysWorld()` |
| [`BulletPhysWorld3D`](documentation/generated/BulletPhysics3D/BulletPhysWorld3D.md)` const&` | `GetPhysWorld()` |
| `bool` | `RaycastQuery(Nz::Vector3f const& from, Nz::Vector3f const& to, FunctionRef<std::optional<float> (const `[`BulletPhysics3DSystem::RaycastHit`](documentation/generated/BulletPhysics3D/BulletPhysics3DSystem.RaycastHit.md)` &)> const& callback)` |
| `bool` | `RaycastQueryFirst(Nz::Vector3f const& from, Nz::Vector3f const& to, `[`BulletPhysics3DSystem::RaycastHit`](documentation/generated/BulletPhysics3D/BulletPhysics3DSystem.RaycastHit.md)`* hitInfo)` |
| `void` | `Update(`[`Time`](documentation/generated/Core/Time.md)` elapsedTime)` |
| BulletPhysics3DSystem`&` | `operator=(`BulletPhysics3DSystem` const&)` |
| BulletPhysics3DSystem`&` | `operator=(`BulletPhysics3DSystem`&&)` |
