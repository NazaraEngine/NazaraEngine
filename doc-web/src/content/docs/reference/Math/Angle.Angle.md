---
title: Angle.Angle
description: Nothing
---

# Nz::Angle::Angle

Class description

## Constructors

- `Angle()`
- `Angle(T angle)`
- `Angle(const Angle<Unit, T>&)`
- `Angle(Angle<Unit, T>&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `ApproxEqual(const Angle<Unit, T>& angle)` |
| `bool` | `ApproxEqual(const Angle<Unit, T>& angle, T maxDifference)` |
| `T` | `GetCos()` |
| `T` | `GetSin()` |
| `std::pair<T, T>` | `GetSinCos()` |
| `T` | `GetTan()` |
| `Angle<Unit, T>&` | `Normalize()` |
| `T` | `ToDegrees()` |
| `Angle<AngleUnit::Degree, T>` | `ToDegreeAngle()` |
| `EulerAngles<T>` | `ToEulerAngles()` |
| `Quaternion<T>` | `ToQuaternion()` |
| `T` | `ToRadians()` |
| `Angle<AngleUnit::Radian, T>` | `ToRadianAngle()` |
| `std::string` | `ToString()` |
| `T` | `ToTurns()` |
| `Angle<AngleUnit::Turn, T>` | `ToTurnAngle()` |
| `Angle<Unit, T>&` | `operator=(const Angle<Unit, T>&)` |
| `Angle<Unit, T>&` | `operator=(Angle<Unit, T>&&)` |
| `Angle<Unit, T>` | `operator+()` |
| `Angle<Unit, T>` | `operator-()` |
| `Angle<Unit, T>` | `operator+(Angle<Unit, T> other)` |
| `Angle<Unit, T>` | `operator-(Angle<Unit, T> other)` |
| `Angle<Unit, T>` | `operator*(T scalar)` |
| `Angle<Unit, T>` | `operator/(T divider)` |
| `Angle<Unit, T>&` | `operator+=(Angle<Unit, T> other)` |
| `Angle<Unit, T>&` | `operator-=(Angle<Unit, T> other)` |
| `Angle<Unit, T>&` | `operator*=(T scalar)` |
| `Angle<Unit, T>&` | `operator/=(T divider)` |
| `bool` | `operator==(Angle<Unit, T> other)` |
| `bool` | `operator!=(Angle<Unit, T> other)` |
| `bool` | `operator<(Angle<Unit, T> other)` |
| `bool` | `operator<=(Angle<Unit, T> other)` |
| `bool` | `operator>(Angle<Unit, T> other)` |
| `bool` | `operator>=(Angle<Unit, T> other)` |
