---
title: BulletRigidBody3D
description: Nothing
---

# Nz::BulletRigidBody3D

Class description

## Constructors

- `BulletRigidBody3D(`[`BulletPhysWorld3D`](documentation/generated/BulletPhysics3D/BulletPhysWorld3D.md)`* world, Nz::Matrix4f const& mat)`
- `BulletRigidBody3D(`[`BulletPhysWorld3D`](documentation/generated/BulletPhysics3D/BulletPhysWorld3D.md)`* world, std::shared_ptr<`[`BulletCollider3D`](documentation/generated/BulletPhysics3D/BulletCollider3D.md)`> geom, Nz::Matrix4f const& mat)`
- `BulletRigidBody3D(`BulletRigidBody3D` const& object)`
- `BulletRigidBody3D(`BulletRigidBody3D`&& object)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `AddForce(Nz::Vector3f const& force, Nz::CoordSys coordSys)` |
| `void` | `AddForce(Nz::Vector3f const& force, Nz::Vector3f const& point, Nz::CoordSys coordSys)` |
| `void` | `AddTorque(Nz::Vector3f const& torque, Nz::CoordSys coordSys)` |
| `void` | `DisableSleeping()` |
| `void` | `EnableSleeping(bool enable)` |
| `void` | `FallAsleep()` |
| `Nz::Boxf` | `GetAABB()` |
| `float` | `GetAngularDamping()` |
| `Nz::Vector3f` | `GetAngularVelocity()` |
| `std::shared_ptr<`[`BulletCollider3D`](documentation/generated/BulletPhysics3D/BulletCollider3D.md)`> const&` | `GetGeom()` |
| `float` | `GetLinearDamping()` |
| `Nz::Vector3f` | `GetLinearVelocity()` |
| `float` | `GetMass()` |
| `Nz::Vector3f` | `GetMassCenter(Nz::CoordSys coordSys)` |
| `Nz::Matrix4f` | `GetMatrix()` |
| `Nz::Vector3f` | `GetPosition()` |
| `btRigidBody*` | `GetRigidBody()` |
| `Nz::Quaternionf` | `GetRotation()` |
| `std::size_t` | `GetUniqueIndex()` |
| [`BulletPhysWorld3D`](documentation/generated/BulletPhysics3D/BulletPhysWorld3D.md)`*` | `GetWorld()` |
| `bool` | `IsSimulationEnabled()` |
| `bool` | `IsSleeping()` |
| `bool` | `IsSleepingEnabled()` |
| `void` | `SetAngularDamping(float angularDamping)` |
| `void` | `SetAngularVelocity(Nz::Vector3f const& angularVelocity)` |
| `void` | `SetGeom(std::shared_ptr<`[`BulletCollider3D`](documentation/generated/BulletPhysics3D/BulletCollider3D.md)`> geom, bool recomputeInertia)` |
| `void` | `SetLinearDamping(float damping)` |
| `void` | `SetLinearVelocity(Nz::Vector3f const& velocity)` |
| `void` | `SetMass(float mass)` |
| `void` | `SetMassCenter(Nz::Vector3f const& center)` |
| `void` | `SetPosition(Nz::Vector3f const& position)` |
| `void` | `SetPositionAndRotation(Nz::Vector3f const& position, Nz::Quaternionf const& rotation)` |
| `void` | `SetRotation(Nz::Quaternionf const& rotation)` |
| `Nz::Quaternionf` | `ToLocal(Nz::Quaternionf const& worldRotation)` |
| `Nz::Vector3f` | `ToLocal(Nz::Vector3f const& worldPosition)` |
| `Nz::Quaternionf` | `ToWorld(Nz::Quaternionf const& localRotation)` |
| `Nz::Vector3f` | `ToWorld(Nz::Vector3f const& localPosition)` |
| `void` | `WakeUp()` |
| BulletRigidBody3D`&` | `operator=(`BulletRigidBody3D` const& object)` |
| BulletRigidBody3D`&` | `operator=(`BulletRigidBody3D`&& object)` |
