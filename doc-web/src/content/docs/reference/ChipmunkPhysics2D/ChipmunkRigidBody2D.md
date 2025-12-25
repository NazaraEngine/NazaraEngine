---
title: ChipmunkRigidBody2D
description: Nothing
---

# Nz::ChipmunkRigidBody2D

Class description

## Constructors

- `ChipmunkRigidBody2D(`[`ChipmunkPhysWorld2D`](documentation/generated/ChipmunkPhysics2D/ChipmunkPhysWorld2D.md)`* world, float mass)`
- `ChipmunkRigidBody2D(`[`ChipmunkPhysWorld2D`](documentation/generated/ChipmunkPhysics2D/ChipmunkPhysWorld2D.md)`* world, float mass, std::shared_ptr<`[`ChipmunkCollider2D`](documentation/generated/ChipmunkPhysics2D/ChipmunkCollider2D.md)`> geom)`
- `ChipmunkRigidBody2D(`ChipmunkRigidBody2D` const& object)`
- `ChipmunkRigidBody2D(`ChipmunkRigidBody2D`&& object)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `AddForce(Nz::Vector2f const& force, Nz::CoordSys coordSys)` |
| `void` | `AddForce(Nz::Vector2f const& force, Nz::Vector2f const& point, Nz::CoordSys coordSys)` |
| `void` | `AddImpulse(Nz::Vector2f const& impulse, Nz::CoordSys coordSys)` |
| `void` | `AddImpulse(Nz::Vector2f const& impulse, Nz::Vector2f const& point, Nz::CoordSys coordSys)` |
| `void` | `AddTorque(Nz::RadianAnglef const& torque)` |
| `bool` | `ClosestPointQuery(Nz::Vector2f const& position, Nz::Vector2f* closestPoint, float* closestDistance)` |
| `void` | `EnableSimulation(bool simulation)` |
| `void` | `ForEachArbiter(std::function<void (ChipmunkArbiter2D &)> callback)` |
| `void` | `ForceSleep()` |
| `Nz::Rectf` | `GetAABB()` |
| `float` | `GetAngularDamping()` |
| `Nz::RadianAnglef` | `GetAngularVelocity()` |
| `Nz::Vector2f` | `GetCenterOfGravity(Nz::CoordSys coordSys)` |
| `float` | `GetElasticity(std::size_t shapeIndex)` |
| `float` | `GetFriction(std::size_t shapeIndex)` |
| `std::shared_ptr<`[`ChipmunkCollider2D`](documentation/generated/ChipmunkPhysics2D/ChipmunkCollider2D.md)`> const&` | `GetGeom()` |
| `cpBody*` | `GetHandle()` |
| `float` | `GetMass()` |
| `Nz::Vector2f` | `GetMassCenter(Nz::CoordSys coordSys)` |
| `float` | `GetMomentOfInertia()` |
| `Nz::Vector2f` | `GetPosition()` |
| `Nz::Vector2f const&` | `GetPositionOffset()` |
| `Nz::RadianAnglef` | `GetRotation()` |
| `std::size_t` | `GetShapeCount()` |
| `std::size_t` | `GetShapeIndex(cpShape* shape)` |
| `Nz::Vector2f` | `GetSurfaceVelocity(std::size_t shapeIndex)` |
| `void*` | `GetUserdata()` |
| `Nz::Vector2f` | `GetVelocity()` |
| `VelocityFunc&` | `GetVelocityFunction()` |
| [`ChipmunkPhysWorld2D`](documentation/generated/ChipmunkPhysics2D/ChipmunkPhysWorld2D.md)`*` | `GetWorld()` |
| `bool` | `IsKinematic()` |
| `bool` | `IsSimulationEnabled()` |
| `bool` | `IsSleeping()` |
| `bool` | `IsStatic()` |
| `void` | `ResetVelocityFunction()` |
| `void` | `SetAngularDamping(float angularDamping)` |
| `void` | `SetAngularVelocity(Nz::RadianAnglef const& angularVelocity)` |
| `void` | `SetElasticity(float elasticity)` |
| `void` | `SetElasticity(std::size_t shapeIndex, float elasticity)` |
| `void` | `SetFriction(float friction)` |
| `void` | `SetFriction(std::size_t shapeIndex, float friction)` |
| `void` | `SetGeom(std::shared_ptr<`[`ChipmunkCollider2D`](documentation/generated/ChipmunkPhysics2D/ChipmunkCollider2D.md)`> geom, bool recomputeMoment, bool recomputeMassCenter)` |
| `void` | `SetMass(float mass, bool recomputeMoment)` |
| `void` | `SetMassCenter(Nz::Vector2f const& center, Nz::CoordSys coordSys)` |
| `void` | `SetMomentOfInertia(float moment)` |
| `void` | `SetPosition(Nz::Vector2f const& position)` |
| `void` | `SetPositionOffset(Nz::Vector2f const& offset)` |
| `void` | `SetRotation(Nz::RadianAnglef const& rotation)` |
| `void` | `SetSurfaceVelocity(Nz::Vector2f const& surfaceVelocity)` |
| `void` | `SetSurfaceVelocity(std::size_t shapeIndex, Nz::Vector2f const& surfaceVelocity)` |
| `void` | `SetStatic(bool setStaticBody)` |
| `void` | `SetUserdata(void* ud)` |
| `void` | `SetVelocity(Nz::Vector2f const& velocity)` |
| `void` | `SetVelocityFunction(Nz::ChipmunkRigidBody2D::VelocityFunc velocityFunc)` |
| `void` | `TeleportTo(Nz::Vector2f const& position, Nz::RadianAnglef const& rotation)` |
| `void` | `UpdateVelocity(Nz::Vector2f const& gravity, float damping, float deltaTime)` |
| `void` | `Wakeup()` |
| ChipmunkRigidBody2D`&` | `operator=(`ChipmunkRigidBody2D` const& object)` |
| ChipmunkRigidBody2D`&` | `operator=(`ChipmunkRigidBody2D`&& object)` |
