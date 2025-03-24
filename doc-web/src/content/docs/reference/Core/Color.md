---
title: Color
description: Nothing
---

# Nz::Color

Class description

## Constructors

- `Color()`
- `Color(float red, float green, float blue, float alpha)`
- `Color(float lightness)`
- `Color(`Color` const&)`
- `Color(`Color`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `IsOpaque()` |
| `std::string` | `ToString()` |
| `Nz::Color` | `operator+(`Color` const& color)` |
| `Nz::Color` | `operator*(`Color` const& color)` |
| Color`&` | `operator=(`Color` const&)` |
| Color`&` | `operator=(`Color`&&)` |
| `Nz::Color` | `operator+=(`Color` const& color)` |
| `Nz::Color` | `operator*=(`Color` const& color)` |
| `bool` | `operator==(`Color` const& color)` |
| `bool` | `operator!=(`Color` const& color)` |
