---
title: Vector3.Vector3
description: Nothing
---

# Nz::Vector3::Vector3

Class description

## Constructors

- `Vector3()`
- `Vector3(T X, T Y, T Z)`
- `Vector3(T X, Vector2<T> const& vec)`
- `Vector3(T scale)`
- `Vector3(Vector2<T> const& vec, T Z)`
- `Vector3(const Vector3<T>&)`
- `Vector3(Vector3<T>&&)`
- `Vector3(Vector4<T> const& vec)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `T` | `AbsDotProduct(const Vector3<T>& vec)` |
| `RadianAngle<T>` | `AngleBetween(const Vector3<T>& vec)` |
| `bool` | `ApproxEqual(const Vector3<T>& vec, T maxDifference)` |
| `Vector3<T>` | `CrossProduct(const Vector3<T>& vec)` |
| `T` | `DotProduct(const Vector3<T>& vec)` |
| `Vector3<T>` | `GetAbs()` |
| `Vector3<T>` | `GetNormal(T* length)` |
| `T` | `GetSquaredLength()` |
| `Vector3<T>&` | `Maximize(const Vector3<T>& vec)` |
| `Vector3<T>&` | `Minimize(const Vector3<T>& vec)` |
| `Vector3<T>&` | `Normalize(T* length)` |
| `T` | `SquaredDistance(const Vector3<T>& vec)` |
| `std::string` | `ToString()` |
| `T&` | `operator[](std::size_t i)` |
| `T const&` | `operator[](std::size_t i)` |
| `const Vector3<T>&` | `operator+()` |
| `Vector3<T>` | `operator-()` |
| `Vector3<T>` | `operator+(const Vector3<T>& vec)` |
| `Vector3<T>` | `operator-(const Vector3<T>& vec)` |
| `Vector3<T>` | `operator*(const Vector3<T>& vec)` |
| `Vector3<T>` | `operator*(T scale)` |
| `Vector3<T>` | `operator/(const Vector3<T>& vec)` |
| `Vector3<T>` | `operator/(T scale)` |
| `Vector3<T>&` | `operator=(const Vector3<T>&)` |
| `Vector3<T>&` | `operator=(Vector3<T>&&)` |
| `Vector3<T>&` | `operator+=(const Vector3<T>& vec)` |
| `Vector3<T>&` | `operator-=(const Vector3<T>& vec)` |
| `Vector3<T>&` | `operator*=(const Vector3<T>& vec)` |
| `Vector3<T>&` | `operator*=(T scale)` |
| `Vector3<T>&` | `operator/=(const Vector3<T>& vec)` |
| `Vector3<T>&` | `operator/=(T scale)` |
| `bool` | `operator==(const Vector3<T>& vec)` |
| `bool` | `operator!=(const Vector3<T>& vec)` |
| `bool` | `operator<(const Vector3<T>& vec)` |
| `bool` | `operator<=(const Vector3<T>& vec)` |
| `bool` | `operator>(const Vector3<T>& vec)` |
| `bool` | `operator>=(const Vector3<T>& vec)` |
