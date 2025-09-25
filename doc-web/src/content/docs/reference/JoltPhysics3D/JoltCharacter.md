---
title: JoltCharacter
description: Nothing
---

# Nz::JoltCharacter

Class description

## Constructors

- `JoltCharacter(`[`JoltPhysWorld3D`](documentation/generated/JoltPhysics3D/JoltPhysWorld3D.md)`& physWorld, `[`JoltCharacter::Settings`](documentation/generated/JoltPhysics3D/JoltCharacter.Settings.md)` const& settings)`
- `JoltCharacter(`JoltCharacter` const&)`
- `JoltCharacter(`JoltCharacter`&& character)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `DisableSleeping()` |
| `void` | `EnableSleeping(bool enable)` |
| `Nz::UInt32` | `GetBodyIndex()` |
| `Nz::Vector3f` | `GetLinearVelocity()` |
| `Nz::Quaternionf` | `GetRotation()` |
| `Nz::Vector3f` | `GetPosition()` |
| `std::pair<Vector3f, Quaternionf>` | `GetPositionAndRotation()` |
| `Nz::Vector3f` | `GetUp()` |
| `bool` | `IsOnGround()` |
| `void` | `SetFriction(float friction)` |
| `void` | `SetImpl(std::shared_ptr<`[`JoltCharacterImpl`](documentation/generated/JoltPhysics3D/JoltCharacterImpl.md)`> characterImpl)` |
| `void` | `SetLinearVelocity(Nz::Vector3f const& linearVel)` |
| `void` | `SetRotation(Nz::Quaternionf const& rotation)` |
| `void` | `SetUp(Nz::Vector3f const& up)` |
| `void` | `TeleportTo(Nz::Vector3f const& position, Nz::Quaternionf const& rotation)` |
| `void` | `WakeUp()` |
| JoltCharacter`&` | `operator=(`JoltCharacter` const&)` |
| JoltCharacter`&` | `operator=(`JoltCharacter`&& character)` |
