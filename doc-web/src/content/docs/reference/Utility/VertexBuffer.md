---
title: VertexBuffer
description: Nothing
---

# Nz::VertexBuffer

Class description

## Constructors

- `VertexBuffer()`
- `VertexBuffer(std::shared_ptr<const `[`VertexDeclaration`](documentation/generated/Utility/VertexDeclaration.md)`> vertexDeclaration, std::shared_ptr<`[`Buffer`](documentation/generated/Utility/Buffer.md)`> buffer)`
- `VertexBuffer(std::shared_ptr<const `[`VertexDeclaration`](documentation/generated/Utility/VertexDeclaration.md)`> vertexDeclaration, std::shared_ptr<`[`Buffer`](documentation/generated/Utility/Buffer.md)`> buffer, Nz::UInt64 offset, Nz::UInt64 size)`
- `VertexBuffer(std::shared_ptr<const `[`VertexDeclaration`](documentation/generated/Utility/VertexDeclaration.md)`> vertexDeclaration, Nz::UInt32 vertexCount, Nz::BufferUsageFlags usage, Nz::BufferFactory const& bufferFactory, void const* initialData)`
- `VertexBuffer(`VertexBuffer` const&)`
- `VertexBuffer(`VertexBuffer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Fill(void const* data, Nz::UInt64 startVertex, Nz::UInt64 length)` |
| `bool` | `FillRaw(void const* data, Nz::UInt64 offset, Nz::UInt64 size)` |
| `std::shared_ptr<`[`Buffer`](documentation/generated/Utility/Buffer.md)`> const&` | `GetBuffer()` |
| `Nz::UInt64` | `GetEndOffset()` |
| `Nz::UInt64` | `GetStartOffset()` |
| `Nz::UInt64` | `GetStride()` |
| `Nz::UInt32` | `GetVertexCount()` |
| `std::shared_ptr<const `[`VertexDeclaration`](documentation/generated/Utility/VertexDeclaration.md)`> const&` | `GetVertexDeclaration()` |
| `bool` | `IsValid()` |
| `void*` | `Map(Nz::UInt64 startVertex, Nz::UInt64 length)` |
| `void*` | `Map(Nz::UInt64 startVertex, Nz::UInt64 length)` |
| `void*` | `MapRaw(Nz::UInt64 offset, Nz::UInt64 size)` |
| `void*` | `MapRaw(Nz::UInt64 offset, Nz::UInt64 size)` |
| `void` | `SetVertexDeclaration(std::shared_ptr<const `[`VertexDeclaration`](documentation/generated/Utility/VertexDeclaration.md)`> vertexDeclaration)` |
| `void` | `Unmap()` |
| VertexBuffer`&` | `operator=(`VertexBuffer` const&)` |
| VertexBuffer`&` | `operator=(`VertexBuffer`&&)` |
