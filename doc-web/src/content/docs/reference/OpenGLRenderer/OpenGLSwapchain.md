---
title: OpenGLSwapchain
description: Nothing
---

# Nz::OpenGLSwapchain

Class description

## Constructors

- `OpenGLSwapchain(`[`OpenGLDevice`](documentation/generated/OpenGLRenderer/OpenGLDevice.md)`& device, `[`WindowHandle`](documentation/generated/Platform/WindowHandle.md)` windowHandle, Nz::Vector2ui const& windowSize, `[`SwapchainParameters`](documentation/generated/Renderer/SwapchainParameters.md)` const& parameters)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::RenderFrame` | `AcquireFrame()` |
| `std::shared_ptr<`[`CommandPool`](documentation/generated/Renderer/CommandPool.md)`>` | `CreateCommandPool(Nz::QueueType queueType)` |
| [`GL::Context`](documentation/generated/OpenGLRenderer/GL.Context.md)`&` | `GetContext()` |
| [`OpenGLFramebuffer`](documentation/generated/OpenGLRenderer/OpenGLFramebuffer.md)` const&` | `GetFramebuffer(std::size_t i)` |
| `std::size_t` | `GetFramebufferCount()` |
| [`OpenGLRenderPass`](documentation/generated/OpenGLRenderer/OpenGLRenderPass.md)` const&` | `GetRenderPass()` |
| `Nz::Vector2ui const&` | `GetSize()` |
| `void` | `NotifyResize(Nz::Vector2ui const& newSize)` |
| `void` | `Present()` |
| [`TransientResources`](documentation/generated/Renderer/TransientResources.md)`&` | `Transient()` |
