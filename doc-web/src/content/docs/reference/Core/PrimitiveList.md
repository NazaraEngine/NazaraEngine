---
title: PrimitiveList
description: Nothing
---

# Nz::PrimitiveList

Class description

## Constructors

- `PrimitiveList()`
- `PrimitiveList(`PrimitiveList` const&)`
- `PrimitiveList(`PrimitiveList`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `AddBox(Nz::Vector3f const& lengths, Nz::Vector3ui const& subdivision, Nz::Matrix4f const& transformMatrix)` |
| `void` | `AddBox(Nz::Vector3f const& lengths, Nz::Vector3ui const& subdivision, Nz::Vector3f const& position, Nz::Quaternionf const& rotation)` |
| `void` | `AddCone(float length, float radius, unsigned int subdivision, Nz::Matrix4f const& transformMatrix)` |
| `void` | `AddCone(float length, float radius, unsigned int subdivision, Nz::Vector3f const& position, Nz::Quaternionf const& rotation)` |
| `void` | `AddCubicSphere(float size, unsigned int subdivision, Nz::Matrix4f const& transformMatrix)` |
| `void` | `AddCubicSphere(float size, unsigned int subdivision, Nz::Vector3f const& position, Nz::Quaternionf const& rotation)` |
| `void` | `AddIcoSphere(float size, unsigned int recursionLevel, Nz::Matrix4f const& transformMatrix)` |
| `void` | `AddIcoSphere(float size, unsigned int recursionLevel, Nz::Vector3f const& position, Nz::Quaternionf const& rotation)` |
| `void` | `AddPlane(Nz::Vector2f const& size, Nz::Vector2ui const& subdivision, Nz::Matrix4f const& transformMatrix)` |
| `void` | `AddPlane(Nz::Vector2f const& size, Nz::Vector2ui const& subdivision, Nz::Planef const& planeInfo)` |
| `void` | `AddPlane(Nz::Vector2f const& size, Nz::Vector2ui const& subdivision, Nz::Vector3f const& position, Nz::Quaternionf const& rotation)` |
| `void` | `AddUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, Nz::Matrix4f const& transformMatrix)` |
| `void` | `AddUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, Nz::Vector3f const& position, Nz::Quaternionf const& rotation)` |
| [`Primitive`](documentation/generated/Core/Primitive.md)`&` | `GetPrimitive(std::size_t i)` |
| [`Primitive`](documentation/generated/Core/Primitive.md)` const&` | `GetPrimitive(std::size_t i)` |
| `std::size_t` | `GetSize()` |
| PrimitiveList`&` | `operator=(`PrimitiveList` const&)` |
| PrimitiveList`&` | `operator=(`PrimitiveList`&&)` |
| [`Primitive`](documentation/generated/Core/Primitive.md)`&` | `operator()(unsigned int i)` |
| [`Primitive`](documentation/generated/Core/Primitive.md)` const&` | `operator()(unsigned int i)` |
