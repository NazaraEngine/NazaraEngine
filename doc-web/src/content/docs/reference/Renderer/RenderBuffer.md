---
title: RenderBuffer
description: Nothing
---

# Nz::RenderBuffer

Class description

## Constructors

- `RenderBuffer(`[`RenderDevice`](documentation/generated/Renderer/RenderDevice.md)`& renderDevice, Nz::BufferType type, Nz::UInt64 size, Nz::BufferUsageFlags usage)`
- `RenderBuffer(`RenderBuffer` const&)`
- `RenderBuffer(`RenderBuffer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`RenderDevice`](documentation/generated/Renderer/RenderDevice.md)`&` | `GetRenderDevice()` |
| [`RenderDevice`](documentation/generated/Renderer/RenderDevice.md)` const&` | `GetRenderDevice()` |
| `void` | `UpdateDebugName(std::string_view name)` |
| RenderBuffer`&` | `operator=(`RenderBuffer` const&)` |
| RenderBuffer`&` | `operator=(`RenderBuffer`&&)` |
