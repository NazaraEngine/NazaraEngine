---
title: UniformBuffer
description: Nothing
---

# Nz::UniformBuffer

Class description

## Constructors

- `UniformBuffer(std::shared_ptr<`[`Buffer`](documentation/generated/Utility/Buffer.md)`> buffer)`
- `UniformBuffer(std::shared_ptr<`[`Buffer`](documentation/generated/Utility/Buffer.md)`> buffer, Nz::UInt64 offset, Nz::UInt64 size)`
- `UniformBuffer(Nz::UInt64 size, Nz::BufferUsageFlags usage, Nz::BufferFactory const& bufferFactory, void const* initialData)`
- `UniformBuffer(`UniformBuffer` const&)`
- `UniformBuffer(`UniformBuffer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Fill(void const* data, Nz::UInt64 offset, Nz::UInt64 size)` |
| `std::shared_ptr<`[`Buffer`](documentation/generated/Utility/Buffer.md)`> const&` | `GetBuffer()` |
| `Nz::UInt64` | `GetEndOffset()` |
| `Nz::UInt64` | `GetStartOffset()` |
| `void*` | `Map(Nz::UInt64 offset, Nz::UInt64 size)` |
| `void*` | `Map(Nz::UInt64 offset, Nz::UInt64 size)` |
| `void` | `Unmap()` |
| UniformBuffer`&` | `operator=(`UniformBuffer` const&)` |
| UniformBuffer`&` | `operator=(`UniformBuffer`&&)` |
