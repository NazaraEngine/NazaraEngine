---
title: BoundingVolume.BoundingVolume
description: Nothing
---

# Nz::BoundingVolume::BoundingVolume

Class description

## Constructors

- `BoundingVolume()`
- `BoundingVolume(Nz::Extent Extend)`
- `BoundingVolume(Box<T> const& box)`
- `BoundingVolume(OrientedBox<T> const& orientedBox)`
- `BoundingVolume(const BoundingVolume<T>&)`
- `BoundingVolume(BoundingVolume<T>&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `ApproxEqual(const BoundingVolume<T>& volume, T maxDifference)` |
| `BoundingVolume<T>&` | `ExtendTo(const BoundingVolume<T>& volume)` |
| `bool` | `Intersect(Box<T> const& box)` |
| `bool` | `IsFinite()` |
| `bool` | `IsInfinite()` |
| `bool` | `IsNull()` |
| `std::string` | `ToString()` |
| `void` | `Update(Matrix4<T> const& transformMatrix)` |
| `void` | `Update(Vector3<T> const& translation)` |
| `BoundingVolume<T>&` | `operator=(const BoundingVolume<T>&)` |
| `BoundingVolume<T>&` | `operator=(BoundingVolume<T>&&)` |
| `BoundingVolume<T>` | `operator*(T scalar)` |
| `BoundingVolume<T>&` | `operator*=(T scalar)` |
| `bool` | `operator==(const BoundingVolume<T>& volume)` |
| `bool` | `operator!=(const BoundingVolume<T>& volume)` |
