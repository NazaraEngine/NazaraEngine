---
title: OpenGLBuffer
description: Nothing
---

# Nz::OpenGLBuffer

Class description

## Constructors

- `OpenGLBuffer(`[`OpenGLDevice`](documentation/generated/OpenGLRenderer/OpenGLDevice.md)`& device, Nz::BufferType type, Nz::UInt64 size, Nz::BufferUsageFlags usage, void const* initialData)`
- `OpenGLBuffer(`OpenGLBuffer` const&)`
- `OpenGLBuffer(`OpenGLBuffer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Fill(void const* data, Nz::UInt64 offset, Nz::UInt64 size)` |
| [`GL::Buffer`](documentation/generated/OpenGLRenderer/GL.Buffer.md)` const&` | `GetBuffer()` |
| `void*` | `Map(Nz::UInt64 offset, Nz::UInt64 size)` |
| `bool` | `Unmap()` |
| `void` | `UpdateDebugName(std::string_view name)` |
| OpenGLBuffer`&` | `operator=(`OpenGLBuffer` const&)` |
| OpenGLBuffer`&` | `operator=(`OpenGLBuffer`&&)` |
