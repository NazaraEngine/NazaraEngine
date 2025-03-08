---
title: OpenGLRenderImage
description: Nothing
---

# Nz::OpenGLRenderImage

Class description

## Constructors

- `OpenGLRenderImage(`[`OpenGLSwapchain`](documentation/generated/OpenGLRenderer/OpenGLSwapchain.md)`& owner)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Execute(FunctionRef<void (Nz::CommandBufferBuilder &)> const& callback, Nz::QueueTypeFlags queueTypeFlags)` |
| [`OpenGLUploadPool`](documentation/generated/OpenGLRenderer/OpenGLUploadPool.md)`&` | `GetUploadPool()` |
| `void` | `Present()` |
| `void` | `SubmitCommandBuffer(`[`CommandBuffer`](documentation/generated/Renderer/CommandBuffer.md)`* commandBuffer, Nz::QueueTypeFlags queueTypeFlags)` |
