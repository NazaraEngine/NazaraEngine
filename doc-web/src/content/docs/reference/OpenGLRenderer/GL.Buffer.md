---
title: GL.Buffer
description: Nothing
---

# Nz::GL::Buffer

Class description

## Constructors

- `Buffer(`GL::Buffer` const&)`
- `Buffer(`GL::Buffer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void*` | `MapRange(GLintptr offset, GLsizeiptr length, GLbitfield access)` |
| `void` | `Reset(Nz::GL::BufferTarget target, GLsizeiptr size, void const* initialData, GLenum usage)` |
| `void` | `SubData(GLintptr offset, GLsizeiptr size, void const* data)` |
| `bool` | `Unmap()` |
| GL::Buffer`&` | `operator=(`GL::Buffer` const&)` |
| GL::Buffer`&` | `operator=(`GL::Buffer`&&)` |
