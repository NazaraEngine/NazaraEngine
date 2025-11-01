---
title: ChipmunkPhysWorld2D
description: Nothing
---

# Nz::ChipmunkPhysWorld2D

Class description

## Constructors

- `ChipmunkPhysWorld2D()`
- `ChipmunkPhysWorld2D(`ChipmunkPhysWorld2D` const&)`
- `ChipmunkPhysWorld2D(`ChipmunkPhysWorld2D`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `DebugDraw(`[`ChipmunkPhysWorld2D::DebugDrawOptions`](documentation/generated/ChipmunkPhysics2D/ChipmunkPhysWorld2D.DebugDrawOptions.md)` const& options, bool drawShapes, bool drawConstraints, bool drawCollisions)` |
| `float` | `GetDamping()` |
| `Nz::Vector2f` | `GetGravity()` |
| `cpSpace*` | `GetHandle()` |
| `std::size_t` | `GetIterationCount()` |
| `std::size_t` | `GetMaxStepCount()` |
| `Nz::Time` | `GetStepSize()` |
| `bool` | `NearestBodyQuery(Nz::Vector2f const& from, float maxDistance, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, `[`ChipmunkRigidBody2D`](documentation/generated/ChipmunkPhysics2D/ChipmunkRigidBody2D.md)`** nearestBody)` |
| `bool` | `NearestBodyQuery(Nz::Vector2f const& from, float maxDistance, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, `[`ChipmunkPhysWorld2D::NearestQueryResult`](documentation/generated/ChipmunkPhysics2D/ChipmunkPhysWorld2D.NearestQueryResult.md)`* result)` |
| `void` | `RaycastQuery(Nz::Vector2f const& from, Nz::Vector2f const& to, float radius, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, FunctionRef<void (const `[`ChipmunkPhysWorld2D::RaycastHit`](documentation/generated/ChipmunkPhysics2D/ChipmunkPhysWorld2D.RaycastHit.md)` &)> const& callback)` |
| `bool` | `RaycastQuery(Nz::Vector2f const& from, Nz::Vector2f const& to, float radius, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, std::vector<RaycastHit>* hitInfos)` |
| `bool` | `RaycastQueryFirst(Nz::Vector2f const& from, Nz::Vector2f const& to, float radius, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, `[`ChipmunkPhysWorld2D::RaycastHit`](documentation/generated/ChipmunkPhysics2D/ChipmunkPhysWorld2D.RaycastHit.md)`* hitInfo)` |
| `void` | `RegionQuery(Nz::Rectf const& boundingBox, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, FunctionRef<void (Nz::ChipmunkRigidBody2D *)> const& callback)` |
| `void` | `RegionQuery(Nz::Rectf const& boundingBox, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, std::vector<`[`ChipmunkRigidBody2D`](documentation/generated/ChipmunkPhysics2D/ChipmunkRigidBody2D.md)` *>* bodies)` |
| `void` | `RegisterCallbacks(unsigned int collisionId, `[`ChipmunkPhysWorld2D::Callback`](documentation/generated/ChipmunkPhysics2D/ChipmunkPhysWorld2D.Callback.md)` callbacks)` |
| `void` | `RegisterCallbacks(unsigned int collisionIdA, unsigned int collisionIdB, `[`ChipmunkPhysWorld2D::Callback`](documentation/generated/ChipmunkPhysics2D/ChipmunkPhysWorld2D.Callback.md)` callbacks)` |
| `void` | `SetDamping(float dampingValue)` |
| `void` | `SetGravity(Nz::Vector2f const& gravity)` |
| `void` | `SetIterationCount(std::size_t iterationCount)` |
| `void` | `SetMaxStepCount(std::size_t maxStepCount)` |
| `void` | `SetSleepTime(`[`Time`](documentation/generated/Core/Time.md)` sleepTime)` |
| `void` | `SetStepSize(`[`Time`](documentation/generated/Core/Time.md)` stepSize)` |
| `void` | `Step(`[`Time`](documentation/generated/Core/Time.md)` timestep)` |
| `void` | `UseSpatialHash(float cellSize, std::size_t entityCount)` |
| ChipmunkPhysWorld2D`&` | `operator=(`ChipmunkPhysWorld2D` const&)` |
| ChipmunkPhysWorld2D`&` | `operator=(`ChipmunkPhysWorld2D`&&)` |
