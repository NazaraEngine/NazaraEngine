---
title: RenderBufferPool
description: Nothing
---

# Nz::RenderBufferPool

Class description

## Constructors

- `RenderBufferPool(std::shared_ptr<`[`RenderDevice`](documentation/generated/Renderer/RenderDevice.md)`> renderDevice, Nz::BufferType bufferType, std::size_t bufferSize, std::size_t bufferPerBlock)`
- `RenderBufferPool(`RenderBufferPool` const&)`
- `RenderBufferPool(`RenderBufferPool`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::RenderBufferView` | `Allocate(std::size_t& index)` |
| `void` | `Free(std::size_t index)` |
| `Nz::UInt64` | `GetBufferAlignedSize()` |
| `Nz::UInt64` | `GetBufferPerBlock()` |
| `Nz::UInt64` | `GetBufferSize()` |
| `Nz::BufferType` | `GetBufferType()` |
| RenderBufferPool`&` | `operator=(`RenderBufferPool` const&)` |
| RenderBufferPool`&` | `operator=(`RenderBufferPool`&&)` |
