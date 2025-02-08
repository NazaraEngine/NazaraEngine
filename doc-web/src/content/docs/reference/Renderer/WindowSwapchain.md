---
title: WindowSwapchain
description: Nothing
---

# Nz::WindowSwapchain

Class description

## Constructors

- `WindowSwapchain(std::shared_ptr<`[`RenderDevice`](documentation/generated/Renderer/RenderDevice.md)`> renderDevice, `[`Window`](documentation/generated/Platform/Window.md)`& window, `[`SwapchainParameters`](documentation/generated/Renderer/SwapchainParameters.md)` parameters)`
- `WindowSwapchain(`WindowSwapchain` const&)`
- `WindowSwapchain(`WindowSwapchain`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::RenderFrame` | `AcquireFrame()` |
| `bool` | `DoesRenderOnlyIfFocused()` |
| `void` | `EnableRenderOnlyIfFocused(bool enable)` |
| [`Framebuffer`](documentation/generated/Renderer/Framebuffer.md)` const&` | `GetFramebuffer(std::size_t i)` |
| `std::size_t` | `GetFramebufferCount()` |
| [`RenderPass`](documentation/generated/Renderer/RenderPass.md)` const&` | `GetRenderPass()` |
| `Nz::Vector2ui const&` | `GetSize()` |
| [`Swapchain`](documentation/generated/Renderer/Swapchain.md)`&` | `GetSwapchain()` |
| [`Swapchain`](documentation/generated/Renderer/Swapchain.md)` const&` | `GetSwapchain()` |
| [`TransientResources`](documentation/generated/Renderer/TransientResources.md)`&` | `Transient()` |
| WindowSwapchain`&` | `operator=(`WindowSwapchain` const&)` |
| WindowSwapchain`&` | `operator=(`WindowSwapchain`&& windowSwapchain)` |
