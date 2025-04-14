---
title: Vector2.Vector2
description: Nothing
---

# Nz::Vector2::Vector2

Class description

## Constructors

- `Vector2()`
- `Vector2(T X, T Y)`
- `Vector2(T scale)`
- `Vector2(const Vector2<T>&)`
- `Vector2(Vector2<T>&&)`
- `Vector2(Vector3<T> const& vec)`
- `Vector2(Vector4<T> const& vec)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `T` | `AbsDotProduct(const Vector2<T>& vec)` |
| `RadianAngle<T>` | `AngleBetween(const Vector2<T>& vec)` |
| `bool` | `ApproxEqual(const Vector2<T>& vec, T maxDifference)` |
| `T` | `DotProduct(const Vector2<T>& vec)` |
| `Vector2<T>` | `GetNormal(T* length)` |
| `T` | `GetSquaredLength()` |
| `Vector2<T>&` | `Maximize(const Vector2<T>& vec)` |
| `Vector2<T>&` | `Minimize(const Vector2<T>& vec)` |
| `Vector2<T>&` | `Normalize(T* length)` |
| `T` | `SquaredDistance(const Vector2<T>& vec)` |
| `std::string` | `ToString()` |
| `T&` | `operator[](std::size_t i)` |
| `T` | `operator[](std::size_t i)` |
| `const Vector2<T>&` | `operator+()` |
| `Vector2<T>` | `operator-()` |
| `Vector2<T>` | `operator+(const Vector2<T>& vec)` |
| `Vector2<T>` | `operator-(const Vector2<T>& vec)` |
| `Vector2<T>` | `operator*(const Vector2<T>& vec)` |
| `Vector2<T>` | `operator*(T scale)` |
| `Vector2<T>` | `operator/(const Vector2<T>& vec)` |
| `Vector2<T>` | `operator/(T scale)` |
| `Vector2<T>&` | `operator=(const Vector2<T>&)` |
| `Vector2<T>&` | `operator=(Vector2<T>&&)` |
| `Vector2<T>&` | `operator+=(const Vector2<T>& vec)` |
| `Vector2<T>&` | `operator-=(const Vector2<T>& vec)` |
| `Vector2<T>&` | `operator*=(const Vector2<T>& vec)` |
| `Vector2<T>&` | `operator*=(T scale)` |
| `Vector2<T>&` | `operator/=(const Vector2<T>& vec)` |
| `Vector2<T>&` | `operator/=(T scale)` |
| `bool` | `operator==(const Vector2<T>& vec)` |
| `bool` | `operator!=(const Vector2<T>& vec)` |
| `bool` | `operator<(const Vector2<T>& vec)` |
| `bool` | `operator<=(const Vector2<T>& vec)` |
| `bool` | `operator>(const Vector2<T>& vec)` |
| `bool` | `operator>=(const Vector2<T>& vec)` |
