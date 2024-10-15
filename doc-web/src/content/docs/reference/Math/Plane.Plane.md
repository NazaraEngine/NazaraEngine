---
title: Plane.Plane
description: Nothing
---

# Nz::Plane::Plane

Class description

## Constructors

- `Plane()`
- `Plane(T normalX, T normalY, T normalZ, T Distance)`
- `Plane(T const[4] plane)`
- `Plane(Vector3<T> const& Normal, T Distance)`
- `Plane(Vector3<T> const& Normal, Vector3<T> const& point)`
- `Plane(Vector3<T> const& point1, Vector3<T> const& point2, Vector3<T> const& point3)`
- `Plane(const Plane<T>& plane)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `ApproxEqual(const Plane<T>& plane, T maxDifference)` |
| `Plane<T>&` | `Normalize(T* length)` |
| `T` | `SignedDistance(Vector3<T> const& point)` |
| `std::string` | `ToString()` |
| `Plane<T>&` | `operator=(const Plane<T>& other)` |
| `bool` | `operator==(const Plane<T>& plane)` |
| `bool` | `operator!=(const Plane<T>& plane)` |
