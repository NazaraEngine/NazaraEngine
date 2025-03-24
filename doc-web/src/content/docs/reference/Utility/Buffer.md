---
title: Buffer
description: Nothing
---

# Nz::Buffer

Class description

## Constructors

- `Buffer(Nz::DataStorage storage, Nz::BufferType type, Nz::UInt64 size, Nz::BufferUsageFlags usage)`
- `Buffer(`Buffer` const&)`
- `Buffer(`Buffer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::shared_ptr<`Buffer`>` | `CopyContent(Nz::BufferFactory const& bufferFactory)` |
| `bool` | `Fill(void const* data, Nz::UInt64 offset, Nz::UInt64 size)` |
| `Nz::UInt64` | `GetSize()` |
| `Nz::DataStorage` | `GetStorage()` |
| `Nz::BufferType` | `GetType()` |
| `Nz::BufferUsageFlags` | `GetUsageFlags()` |
| `void*` | `Map(Nz::UInt64 offset, Nz::UInt64 size)` |
| `bool` | `Unmap()` |
| Buffer`&` | `operator=(`Buffer` const&)` |
| Buffer`&` | `operator=(`Buffer`&&)` |
