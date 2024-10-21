---
title: Uuid
description: Nothing
---

# Nz::Uuid

Class description

## Constructors

- `Uuid()`
- `Uuid(std::array<UInt8, 16> const& uuid)`
- `Uuid(`Uuid` const&)`
- `Uuid(`Uuid`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `IsNull()` |
| `std::array<UInt8, 16> const&` | `ToArray()` |
| `std::string` | `ToString()` |
| `std::array<char, 37>` | `ToStringArray()` |
| Uuid`&` | `operator=(`Uuid` const&)` |
| Uuid`&` | `operator=(`Uuid`&&)` |
