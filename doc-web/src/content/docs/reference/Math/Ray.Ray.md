---
title: Ray.Ray
description: Nothing
---

# Nz::Ray::Ray

Class description

## Constructors

- `Ray()`
- `Ray(T X, T Y, T Z, T directionX, T directionY, T directionZ)`
- `Ray(Vector3<T> const& origin, Vector3<T> const& direction)`
- `Ray(T const[3] origin, T const[3] direction)`
- `Ray(Plane<T> const& planeOne, Plane<T> const& planeTwo)`
- `Ray(const Ray<T>&)`
- `Ray(Ray<T>&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `ApproxEqual(const Ray<T>& ray, T maxDifference)` |
| `T` | `ClosestPoint(Vector3<T> const& point)` |
| `Vector3<T>` | `GetPoint(T lambda)` |
| `bool` | `Intersect(BoundingVolume<T> const& volume, T* closestHit, T* furthestHit)` |
| `bool` | `Intersect(Box<T> const& box, T* closestHit, T* furthestHit)` |
| `bool` | `Intersect(Box<T> const& box, Matrix4<T> const& transform, T* closestHit, T* furthestHit)` |
| `bool` | `Intersect(Plane<T> const& plane, T* hit)` |
| `bool` | `Intersect(Sphere<T> const& sphere, T* closestHit, T* furthestHit)` |
| `bool` | `Intersect(Vector3<T> const& firstPoint, Vector3<T> const& secondPoint, Vector3<T> const& thirdPoint, T* hit)` |
| `std::string` | `ToString()` |
| `Ray<T>&` | `operator=(const Ray<T>&)` |
| `Ray<T>&` | `operator=(Ray<T>&&)` |
| `Vector3<T>` | `operator*(T lambda)` |
| `bool` | `operator==(const Ray<T>& ray)` |
| `bool` | `operator!=(const Ray<T>& ray)` |
| `bool` | `operator<(const Ray<T>& ray)` |
| `bool` | `operator<=(const Ray<T>& ray)` |
| `bool` | `operator>(const Ray<T>& ray)` |
| `bool` | `operator>=(const Ray<T>& ray)` |
