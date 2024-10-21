---
title: SoftwareBuffer
description: Nothing
---

# Nz::SoftwareBuffer

Class description

## Constructors

- `SoftwareBuffer(Nz::BufferType type, Nz::UInt64 size, Nz::BufferUsageFlags usage, void const* initialData)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Fill(void const* data, Nz::UInt64 offset, Nz::UInt64 size)` |
| `Nz::UInt8 const*` | `GetData()` |
| `void*` | `Map(Nz::UInt64 offset, Nz::UInt64 size)` |
| `bool` | `Unmap()` |
