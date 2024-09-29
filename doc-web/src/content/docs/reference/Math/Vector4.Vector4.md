---
title: Vector4.Vector4
description: Nothing
---

# Nz::Vector4::Vector4

Class description

## Constructors

- `Vector4()`
- `Vector4(T X, T Y, T Z, T W)`
- `Vector4(T X, T Y, Vector2<T> const& vec)`
- `Vector4(T X, Vector2<T> const& vec, T W)`
- `Vector4(T X, Vector3<T> const& vec)`
- `Vector4(T scale)`
- `Vector4(Vector2<T> const& vec, T Z, T W)`
- `Vector4(Vector3<T> const& vec, T W)`
- `Vector4(const Vector4<T>&)`
- `Vector4(Vector4<T>&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `T` | `AbsDotProduct(const Vector4<T>& vec)` |
| `bool` | `ApproxEqual(const Vector4<T>& vec, T maxDifference)` |
| `T` | `DotProduct(const Vector4<T>& vec)` |
| `Vector4<T>` | `GetNormal(T* length)` |
| `Vector4<T>&` | `Maximize(const Vector4<T>& vec)` |
| `Vector4<T>&` | `Minimize(const Vector4<T>& vec)` |
| `Vector4<T>&` | `Normalize(T* length)` |
| `std::string` | `ToString()` |
| `Vector4<T>&` | `operator=(const Vector4<T>&)` |
| `Vector4<T>&` | `operator=(Vector4<T>&&)` |
| `T&` | `operator[](std::size_t i)` |
| `T const&` | `operator[](std::size_t i)` |
| `const Vector4<T>&` | `operator+()` |
| `Vector4<T>` | `operator-()` |
| `Vector4<T>` | `operator+(const Vector4<T>& vec)` |
| `Vector4<T>` | `operator-(const Vector4<T>& vec)` |
| `Vector4<T>` | `operator*(const Vector4<T>& vec)` |
| `Vector4<T>` | `operator*(T scale)` |
| `Vector4<T>` | `operator/(const Vector4<T>& vec)` |
| `Vector4<T>` | `operator/(T scale)` |
| `Vector4<T>&` | `operator+=(const Vector4<T>& vec)` |
| `Vector4<T>&` | `operator-=(const Vector4<T>& vec)` |
| `Vector4<T>&` | `operator*=(const Vector4<T>& vec)` |
| `Vector4<T>&` | `operator*=(T scale)` |
| `Vector4<T>&` | `operator/=(const Vector4<T>& vec)` |
| `Vector4<T>&` | `operator/=(T scale)` |
| `bool` | `operator==(const Vector4<T>& vec)` |
| `bool` | `operator!=(const Vector4<T>& vec)` |
| `bool` | `operator<(const Vector4<T>& vec)` |
| `bool` | `operator<=(const Vector4<T>& vec)` |
| `bool` | `operator>(const Vector4<T>& vec)` |
| `bool` | `operator>=(const Vector4<T>& vec)` |
