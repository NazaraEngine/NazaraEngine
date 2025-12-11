---
title: RenderBufferView
description: Nothing
---

# Nz::RenderBufferView

Class description

## Constructors

- `RenderBufferView()`
- `RenderBufferView(`[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)`* buffer)`
- `RenderBufferView(`[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)`* buffer, Nz::UInt64 offset, Nz::UInt64 size)`
- `RenderBufferView(`RenderBufferView` const&)`
- `RenderBufferView(`RenderBufferView`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)`*` | `GetBuffer()` |
| `Nz::UInt64` | `GetOffset()` |
| `Nz::UInt64` | `GetSize()` |
| `bool` | `operator==(`RenderBufferView` const& rhs)` |
| `bool` | `operator!=(`RenderBufferView` const& rhs)` |
| RenderBufferView`&` | `operator=(`RenderBufferView` const&)` |
| RenderBufferView`&` | `operator=(`RenderBufferView`&&)` |
