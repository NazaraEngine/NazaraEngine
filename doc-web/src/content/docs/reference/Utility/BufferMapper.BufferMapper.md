---
title: BufferMapper.BufferMapper
description: Nothing
---

# Nz::BufferMapper::BufferMapper

Class description

## Constructors

- `BufferMapper()`
- `BufferMapper(T& buffer, Nz::UInt64 offset, Nz::UInt64 length)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Map(T& buffer, Nz::UInt64 offset, Nz::UInt64 length)` |
| `T const*` | `GetBuffer()` |
| `void*` | `GetPointer()` |
| `void` | `Unmap()` |
