---
title: Frustum.Frustum
description: Nothing
---

# Nz::Frustum::Frustum

Class description

## Constructors

- `Frustum()`
- `Frustum(EnumArray<Nz::FrustumPlane, Plane<T>> const& planes)`
- `Frustum(const Frustum<T>&)`
- `Frustum(Frustum<T>&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `ApproxEqual(const Frustum<T>& frustum, T maxDifference)` |
| `Vector3<T>` | `ComputeCorner(Nz::BoxCorner corner)` |
| `bool` | `Contains(BoundingVolume<T> const& volume)` |
| `bool` | `Contains(Box<T> const& box)` |
| `bool` | `Contains(OrientedBox<T> const& orientedBox)` |
| `bool` | `Contains(Sphere<T> const& sphere)` |
| `bool` | `Contains(Vector3<T> const& point)` |
| `bool` | `Contains(Vector3<T> const* points, std::size_t pointCount)` |
| `Plane<T> const&` | `GetPlane(Nz::FrustumPlane plane)` |
| `EnumArray<Nz::FrustumPlane, Plane<T>> const&` | `GetPlanes()` |
| `Nz::IntersectionSide` | `Intersect(BoundingVolume<T> const& volume)` |
| `Nz::IntersectionSide` | `Intersect(Box<T> const& box)` |
| `Nz::IntersectionSide` | `Intersect(OrientedBox<T> const& orientedBox)` |
| `Nz::IntersectionSide` | `Intersect(Sphere<T> const& sphere)` |
| `Nz::IntersectionSide` | `Intersect(Vector3<T> const* points, std::size_t pointCount)` |
| `std::string` | `ToString()` |
| `Frustum<T>&` | `operator=(const Frustum<T>&)` |
| `Frustum<T>&` | `operator=(Frustum<T>&&)` |
| `bool` | `operator==(const Frustum<T>& angles)` |
| `bool` | `operator!=(const Frustum<T>& angles)` |
