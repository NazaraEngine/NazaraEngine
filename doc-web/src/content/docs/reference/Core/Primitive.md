---
title: Primitive
description: Nothing
---

# Nz::Primitive

Class description

## Constructors


## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `MakeBox(Nz::Vector3f const& lengths, Nz::Vector3ui const& subdivision, Nz::Matrix4f const& transformMatrix, Nz::Rectf const& uvCoords)` |
| `void` | `MakeBox(Nz::Vector3f const& lengths, Nz::Vector3ui const& subdivision, Nz::Vector3f const& position, Nz::Quaternionf const& rotation, Nz::Rectf const& uvCoords)` |
| `void` | `MakeCone(float length, float radius, unsigned int subdivision, Nz::Matrix4f const& transformMatrix, Nz::Rectf const& uvCoords)` |
| `void` | `MakeCone(float length, float radius, unsigned int subdivision, Nz::Vector3f const& position, Nz::Quaternionf const& rotation, Nz::Rectf const& uvCoords)` |
| `void` | `MakeCubicSphere(float size, unsigned int subdivision, Nz::Matrix4f const& transformMatrix, Nz::Rectf const& uvCoords)` |
| `void` | `MakeCubicSphere(float size, unsigned int subdivision, Nz::Vector3f const& position, Nz::Quaternionf const& rotation, Nz::Rectf const& uvCoords)` |
| `void` | `MakeIcoSphere(float size, unsigned int recursionLevel, Nz::Matrix4f const& transformMatrix, Nz::Rectf const& uvCoords)` |
| `void` | `MakeIcoSphere(float size, unsigned int recursionLevel, Nz::Vector3f const& position, Nz::Quaternionf const& rotation, Nz::Rectf const& uvCoords)` |
| `void` | `MakePlane(Nz::Vector2f const& size, Nz::Vector2ui const& subdivision, Nz::Matrix4f const& transformMatrix, Nz::Rectf const& uvCoords)` |
| `void` | `MakePlane(Nz::Vector2f const& size, Nz::Vector2ui const& subdivision, Nz::Planef const& plane, Nz::Rectf const& uvCoords)` |
| `void` | `MakePlane(Nz::Vector2f const& size, Nz::Vector2ui const& subdivision, Nz::Vector3f const& position, Nz::Quaternionf const& rotation, Nz::Rectf const& uvCoords)` |
| `void` | `MakeUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, Nz::Matrix4f const& transformMatrix, Nz::Rectf const& uvCoords)` |
| `void` | `MakeUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, Nz::Vector3f const& position, Nz::Quaternionf const& rotation, Nz::Rectf const& uvCoords)` |
