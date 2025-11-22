---
title: IndexMapper
description: Nothing
---

# Nz::IndexMapper

Class description

## Constructors

- `IndexMapper(`[`IndexBuffer`](documentation/generated/Utility/IndexBuffer.md)`& indexBuffer, Nz::UInt32 indexCount)`
- `IndexMapper(`[`SubMesh`](documentation/generated/Utility/SubMesh.md)`& subMesh)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::UInt32` | `Get(std::size_t i)` |
| [`IndexBuffer`](documentation/generated/Utility/IndexBuffer.md)` const*` | `GetBuffer()` |
| `Nz::UInt32` | `GetIndexCount()` |
| `void` | `Set(std::size_t i, Nz::UInt32 value)` |
| `void` | `Unmap()` |
| `Nz::IndexIterator` | `begin()` |
| `Nz::IndexIterator` | `end()` |
