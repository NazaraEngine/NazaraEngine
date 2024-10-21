---
title: OpenGLUploadPool
description: Nothing
---

# Nz::OpenGLUploadPool

Class description

## Constructors

- `OpenGLUploadPool(Nz::UInt64 blockSize)`
- `OpenGLUploadPool(`OpenGLUploadPool` const&)`
- `OpenGLUploadPool(`OpenGLUploadPool`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`UploadPool::Allocation`](documentation/generated/Renderer/UploadPool.Allocation.md)`&` | `Allocate(Nz::UInt64 size)` |
| [`UploadPool::Allocation`](documentation/generated/Renderer/UploadPool.Allocation.md)`&` | `Allocate(Nz::UInt64 size, Nz::UInt64 alignment)` |
| `void` | `Reset()` |
| OpenGLUploadPool`&` | `operator=(`OpenGLUploadPool` const&)` |
| OpenGLUploadPool`&` | `operator=(`OpenGLUploadPool`&&)` |
