---
title: RenderSystem
description: Nothing
---

# Nz::RenderSystem

Class description

## Constructors

- `RenderSystem(entt::registry& registry)`
- `RenderSystem(`RenderSystem` const&)`
- `RenderSystem(`RenderSystem`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`WindowSwapchain`](documentation/generated/Renderer/WindowSwapchain.md)`&` | `CreateSwapchain(`[`Window`](documentation/generated/Platform/Window.md)`& window, `[`SwapchainParameters`](documentation/generated/Renderer/SwapchainParameters.md)` const& parameters)` |
| [`FramePipeline`](documentation/generated/Graphics/FramePipeline.md)`&` | `GetFramePipeline()` |
| [`FramePipeline`](documentation/generated/Graphics/FramePipeline.md)` const&` | `GetFramePipeline()` |
| `void` | `Update(`[`Time`](documentation/generated/Core/Time.md)` elapsedTime)` |
| RenderSystem`&` | `operator=(`RenderSystem` const&)` |
| RenderSystem`&` | `operator=(`RenderSystem`&&)` |
