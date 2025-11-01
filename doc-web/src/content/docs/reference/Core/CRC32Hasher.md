---
title: CRC32Hasher
description: Nothing
---

# Nz::CRC32Hasher

Class description

## Constructors

- `CRC32Hasher(Nz::UInt32 polynomial)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Append(Nz::UInt8 const* data, std::size_t len)` |
| `void` | `Begin()` |
| `Nz::ByteArray` | `End()` |
| `std::size_t` | `GetDigestLength()` |
| `char const*` | `GetHashName()` |
