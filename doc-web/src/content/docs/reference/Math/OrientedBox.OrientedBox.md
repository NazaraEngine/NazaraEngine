---
title: OrientedBox.OrientedBox
description: Nothing
---

# Nz::OrientedBox::OrientedBox

Class description

## Constructors

- `OrientedBox()`
- `OrientedBox(Box<T> const& box)`
- `OrientedBox(const OrientedBox<T>&)`
- `OrientedBox(OrientedBox<T>&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `ApproxEqual(const OrientedBox<T>& obb, T maxDifference)` |
| `Vector3<T> const&` | `GetCorner(Nz::BoxCorner corner)` |
| `Vector3<T> const*` | `GetCorners()` |
| `bool` | `IsValid()` |
| `std::string` | `ToString()` |
| `void` | `Update(Matrix4<T> const& transformMatrix)` |
| `void` | `Update(Vector3<T> const& transformMatrix)` |
| `Vector3<T>&` | `operator()(unsigned int i)` |
| `Vector3<T> const&` | `operator()(unsigned int i)` |
| `OrientedBox<T>&` | `operator=(const OrientedBox<T>&)` |
| `OrientedBox<T>&` | `operator=(OrientedBox<T>&&)` |
| `bool` | `operator==(const OrientedBox<T>& box)` |
| `bool` | `operator!=(const OrientedBox<T>& box)` |
