---
title: ChipmunkSegmentCollider2D
description: Nothing
---

# Nz::ChipmunkSegmentCollider2D

Class description

## Constructors

- `ChipmunkSegmentCollider2D(Nz::Vector2f const& first, Nz::Vector2f const& second, float thickness)`
- `ChipmunkSegmentCollider2D(Nz::Vector2f const& first, Nz::Vector2f const& firstNeighbor, Nz::Vector2f const& second, Nz::Vector2f const& secondNeighbor, float thickness)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::Vector2f` | `ComputeCenterOfMass()` |
| `float` | `ComputeMomentOfInertia(float mass)` |
| `Nz::Vector2f const&` | `GetFirstPoint()` |
| `Nz::Vector2f const&` | `GetFirstPointNeighbor()` |
| `float` | `GetLength()` |
| `Nz::Vector2f const&` | `GetSecondPoint()` |
| `Nz::Vector2f const&` | `GetSecondPointNeighbor()` |
| `float` | `GetThickness()` |
| `Nz::ChipmunkColliderType2D` | `GetType()` |
