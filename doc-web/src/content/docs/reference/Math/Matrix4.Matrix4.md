---
title: Matrix4.Matrix4
description: Nothing
---

# Nz::Matrix4::Matrix4

Class description

## Constructors

- `Matrix4()`
- `Matrix4(T r11, T r12, T r13, T r14, T r21, T r22, T r23, T r24, T r31, T r32, T r33, T r34, T r41, T r42, T r43, T r44)`
- `Matrix4(T const[16] matrix)`
- `Matrix4(const Matrix4<T>&)`
- `Matrix4(Matrix4<T>&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Matrix4<T>&` | `ApplyRotation(Quaternion<T> const& rotation)` |
| `Matrix4<T>&` | `ApplyScale(Vector3<T> const& scale)` |
| `Matrix4<T>&` | `ApplyTranslation(Vector3<T> const& translation)` |
| `bool` | `ApproxEqual(const Matrix4<T>& vec, T maxDifference)` |
| `Matrix4<T>&` | `Concatenate(const Matrix4<T>& matrix)` |
| `Matrix4<T>&` | `ConcatenateTransform(const Matrix4<T>& matrix)` |
| `Vector4<T>` | `GetColumn(std::size_t column)` |
| `T` | `GetDeterminant()` |
| `T` | `GetDeterminantTransform()` |
| `bool` | `GetInverse(Matrix4<T>* dest)` |
| `bool` | `GetInverseTransform(Matrix4<T>* dest)` |
| `Quaternion<T>` | `GetRotation()` |
| `Vector4<T>` | `GetRow(std::size_t row)` |
| `Vector3<T>` | `GetScale()` |
| `Vector3<T>` | `GetSquaredScale()` |
| `Vector3<T>` | `GetTranslation()` |
| `void` | `GetTransposed(Matrix4<T>* dest)` |
| `bool` | `HasNegativeScale()` |
| `bool` | `HasScale()` |
| `Matrix4<T>&` | `Inverse(bool* succeeded)` |
| `Matrix4<T>&` | `InverseTransform(bool* succeeded)` |
| `bool` | `IsTransformMatrix()` |
| `bool` | `IsIdentity()` |
| `Matrix4<T>&` | `SetRotation(Quaternion<T> const& rotation)` |
| `Matrix4<T>&` | `SetScale(Vector3<T> const& scale)` |
| `Matrix4<T>&` | `SetTranslation(Vector3<T> const& translation)` |
| `std::string` | `ToString()` |
| `Vector2<T>` | `Transform(Vector2<T> const& vector, T z, T w)` |
| `Vector3<T>` | `Transform(Vector3<T> const& vector, T w)` |
| `Vector4<T>` | `Transform(Vector4<T> const& vector)` |
| `Matrix4<T>&` | `Transpose()` |
| `T&` | `operator()(std::size_t x, std::size_t y)` |
| `T const&` | `operator()(std::size_t x, std::size_t y)` |
| `T&` | `operator[](std::size_t i)` |
| `T const&` | `operator[](std::size_t i)` |
| `Matrix4<T>&` | `operator=(const Matrix4<T>&)` |
| `Matrix4<T>&` | `operator=(Matrix4<T>&&)` |
| `Matrix4<T>` | `operator*(const Matrix4<T>& matrix)` |
| `Vector2<T>` | `operator*(Vector2<T> const& vector)` |
| `Vector3<T>` | `operator*(Vector3<T> const& vector)` |
| `Vector4<T>` | `operator*(Vector4<T> const& vector)` |
| `Matrix4<T>` | `operator*(T scalar)` |
| `Matrix4<T>&` | `operator*=(const Matrix4<T>& matrix)` |
| `Matrix4<T>&` | `operator*=(T scalar)` |
| `bool` | `operator==(const Matrix4<T>& mat)` |
| `bool` | `operator!=(const Matrix4<T>& mat)` |
