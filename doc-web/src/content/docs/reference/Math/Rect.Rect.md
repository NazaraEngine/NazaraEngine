---
title: Rect.Rect
description: Nothing
---

# Nz::Rect::Rect

Class description

## Constructors

- `Rect()`
- `Rect(T Width, T Height)`
- `Rect(T X, T Y, T Width, T Height)`
- `Rect(Vector2<T> const& lengths)`
- `Rect(Vector2<T> const& pos, Vector2<T> const& lengths)`
- `Rect(const Rect<T>&)`
- `Rect(Rect<T>&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `ApproxEqual(const Rect<T>& rect, T maxDifference)` |
| `bool` | `Contains(T X, T Y)` |
| `bool` | `Contains(const Rect<T>& rect)` |
| `bool` | `Contains(Vector2<T> const& point)` |
| `Rect<T>&` | `ExtendTo(T X, T Y)` |
| `Rect<T>&` | `ExtendTo(const Rect<T>& rect)` |
| `Rect<T>&` | `ExtendTo(Vector2<T> const& point)` |
| `Vector2<T>` | `GetCenter()` |
| `Vector2<T>` | `GetCorner(Nz::RectCorner corner)` |
| `Vector2<T>` | `GetLengths()` |
| `Vector2<T>` | `GetMaximum()` |
| `Vector2<T>` | `GetMinimum()` |
| `Vector2<T>` | `GetNegativeVertex(Vector2<T> const& normal)` |
| `Vector2<T>` | `GetPosition()` |
| `Vector2<T>` | `GetPositiveVertex(Vector2<T> const& normal)` |
| `bool` | `Intersect(const Rect<T>& rect, Rect<T>* intersection)` |
| `bool` | `IsNull()` |
| `bool` | `IsValid()` |
| `Rect<T>&` | `Scale(T scalar)` |
| `Rect<T>&` | `Scale(Vector2<T> const& vec)` |
| `Rect<T>&` | `ScaleAroundCenter(T scalar)` |
| `Rect<T>&` | `ScaleAroundCenter(Vector2<T> const& vec)` |
| `std::string` | `ToString()` |
| `Rect<T>&` | `Translate(Vector2<T> const& translation)` |
| `T&` | `operator[](std::size_t i)` |
| `T const&` | `operator[](std::size_t i)` |
| `Rect<T>&` | `operator=(const Rect<T>&)` |
| `Rect<T>&` | `operator=(Rect<T>&&)` |
| `bool` | `operator==(const Rect<T>& rect)` |
| `bool` | `operator!=(const Rect<T>& rect)` |
