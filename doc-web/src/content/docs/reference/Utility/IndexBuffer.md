---
title: IndexBuffer
description: Nothing
---

# Nz::IndexBuffer

Class description

## Constructors

- `IndexBuffer()`
- `IndexBuffer(Nz::IndexType indexType, std::shared_ptr<`[`Buffer`](documentation/generated/Utility/Buffer.md)`> buffer)`
- `IndexBuffer(Nz::IndexType indexType, std::shared_ptr<`[`Buffer`](documentation/generated/Utility/Buffer.md)`> buffer, Nz::UInt64 offset, Nz::UInt64 size)`
- `IndexBuffer(Nz::IndexType indexType, Nz::UInt32 indexCount, Nz::BufferUsageFlags usage, Nz::BufferFactory const& bufferFactory, void const* initialData)`
- `IndexBuffer(`IndexBuffer` const&)`
- `IndexBuffer(`IndexBuffer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::UInt64` | `ComputeCacheMissCount()` |
| `bool` | `Fill(void const* data, Nz::UInt64 startIndex, Nz::UInt64 length)` |
| `bool` | `FillRaw(void const* data, Nz::UInt64 offset, Nz::UInt64 size)` |
| `std::shared_ptr<`[`Buffer`](documentation/generated/Utility/Buffer.md)`> const&` | `GetBuffer()` |
| `Nz::UInt64` | `GetEndOffset()` |
| `Nz::UInt32` | `GetIndexCount()` |
| `Nz::IndexType` | `GetIndexType()` |
| `Nz::UInt64` | `GetStride()` |
| `Nz::UInt64` | `GetStartOffset()` |
| `bool` | `IsValid()` |
| `void*` | `Map(Nz::UInt64 startIndex, Nz::UInt64 length)` |
| `void*` | `Map(Nz::UInt64 startIndex, Nz::UInt64 length)` |
| `void*` | `MapRaw(Nz::UInt64 offset, Nz::UInt64 size)` |
| `void*` | `MapRaw(Nz::UInt64 offset, Nz::UInt64 size)` |
| `void` | `Optimize()` |
| `void` | `Unmap()` |
| IndexBuffer`&` | `operator=(`IndexBuffer` const&)` |
| IndexBuffer`&` | `operator=(`IndexBuffer`&&)` |
