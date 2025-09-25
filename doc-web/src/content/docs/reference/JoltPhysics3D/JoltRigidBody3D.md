---
title: JoltRigidBody3D
description: Nothing
---

# Nz::JoltRigidBody3D

Class description

## Constructors

- `JoltRigidBody3D(`[`JoltPhysWorld3D`](documentation/generated/JoltPhysics3D/JoltPhysWorld3D.md)`& world, `[`JoltRigidBody3D::DynamicSettings`](documentation/generated/JoltPhysics3D/JoltRigidBody3D.DynamicSettings.md)` const& settings)`
- `JoltRigidBody3D(`[`JoltPhysWorld3D`](documentation/generated/JoltPhysics3D/JoltPhysWorld3D.md)`& world, `[`JoltRigidBody3D::StaticSettings`](documentation/generated/JoltPhysics3D/JoltRigidBody3D.StaticSettings.md)` const& settings)`
- `JoltRigidBody3D(`JoltRigidBody3D` const& object)`
- `JoltRigidBody3D(`JoltRigidBody3D`&& object)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `AddForce(Nz::Vector3f const& force, Nz::CoordSys coordSys)` |
| `void` | `AddForce(Nz::Vector3f const& force, Nz::Vector3f const& point, Nz::CoordSys coordSys)` |
| `void` | `AddTorque(Nz::Vector3f const& torque, Nz::CoordSys coordSys)` |
| `void` | `DisableSimulation()` |
| `void` | `DisableSleeping()` |
| `void` | `EnableSimulation(bool enable)` |
| `void` | `EnableSleeping(bool enable)` |
| `void` | `FallAsleep()` |
| `Nz::Boxf` | `GetAABB()` |
| `float` | `GetAngularDamping()` |
| `Nz::Vector3f` | `GetAngularVelocity()` |
| `JPH::Body*` | `GetBody()` |
| `JPH::Body const*` | `GetBody()` |
| `Nz::UInt32` | `GetBodyIndex()` |
| `std::shared_ptr<`[`JoltCollider3D`](documentation/generated/JoltPhysics3D/JoltCollider3D.md)`> const&` | `GetGeom()` |
| `float` | `GetLinearDamping()` |
| `Nz::Vector3f` | `GetLinearVelocity()` |
| `float` | `GetMass()` |
| `Nz::Matrix4f` | `GetMatrix()` |
| `Nz::Vector3f` | `GetPosition()` |
| `std::pair<Vector3f, Quaternionf>` | `GetPositionAndRotation()` |
| `Nz::Quaternionf` | `GetRotation()` |
| [`JoltPhysWorld3D`](documentation/generated/JoltPhysics3D/JoltPhysWorld3D.md)`&` | `GetWorld()` |
| `bool` | `IsSimulationEnabled()` |
| `bool` | `IsSleeping()` |
| `bool` | `IsSleepingEnabled()` |
| `bool` | `IsStatic()` |
| `void` | `SetAngularDamping(float angularDamping)` |
| `void` | `SetAngularVelocity(Nz::Vector3f const& angularVelocity)` |
| `void` | `SetGeom(std::shared_ptr<`[`JoltCollider3D`](documentation/generated/JoltPhysics3D/JoltCollider3D.md)`> geom, bool recomputeInertia)` |
| `void` | `SetLinearDamping(float damping)` |
| `void` | `SetLinearVelocity(Nz::Vector3f const& velocity)` |
| `void` | `SetMass(float mass, bool recomputeInertia)` |
| `void` | `SetPosition(Nz::Vector3f const& position)` |
| `void` | `SetRotation(Nz::Quaternionf const& rotation)` |
| `void` | `TeleportTo(Nz::Vector3f const& position, Nz::Quaternionf const& rotation)` |
| `Nz::Quaternionf` | `ToLocal(Nz::Quaternionf const& worldRotation)` |
| `Nz::Vector3f` | `ToLocal(Nz::Vector3f const& worldPosition)` |
| `Nz::Quaternionf` | `ToWorld(Nz::Quaternionf const& localRotation)` |
| `Nz::Vector3f` | `ToWorld(Nz::Vector3f const& localPosition)` |
| `void` | `WakeUp()` |
| JoltRigidBody3D`&` | `operator=(`JoltRigidBody3D` const& object)` |
| JoltRigidBody3D`&` | `operator=(`JoltRigidBody3D`&& object)` |
