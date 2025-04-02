---
title: RenderFrame
description: Nothing
---

# Nz::RenderFrame

Class description

## Constructors

- `RenderFrame()`
- `RenderFrame(`[`RenderImage`](documentation/generated/Renderer/RenderImage.md)`* renderImage, bool framebufferInvalidation, Nz::Vector2ui const& size, std::size_t framebufferIndex)`
- `RenderFrame(`RenderFrame` const&)`
- `RenderFrame(`RenderFrame`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Execute(FunctionRef<void (Nz::CommandBufferBuilder &)> const& callback, Nz::QueueTypeFlags queueTypeFlags)` |
| `std::size_t` | `GetFramebufferIndex()` |
| `Nz::Vector2ui const&` | `GetSize()` |
| [`UploadPool`](documentation/generated/Renderer/UploadPool.md)`&` | `GetUploadPool()` |
| `bool` | `IsFramebufferInvalidated()` |
| `void` | `Present()` |
| `void` | `SubmitCommandBuffer(`[`CommandBuffer`](documentation/generated/Renderer/CommandBuffer.md)`* commandBuffer, Nz::QueueTypeFlags queueTypeFlags)` |
| RenderFrame`&` | `operator=(`RenderFrame` const&)` |
| RenderFrame`&` | `operator=(`RenderFrame`&&)` |
