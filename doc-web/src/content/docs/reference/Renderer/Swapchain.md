---
title: Swapchain
description: Nothing
---

# Nz::Swapchain

Class description

## Constructors

- `Swapchain()`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::RenderFrame` | `AcquireFrame()` |
| `std::shared_ptr<`[`CommandPool`](documentation/generated/Renderer/CommandPool.md)`>` | `CreateCommandPool(Nz::QueueType queueType)` |
| `void` | `NotifyResize(Nz::Vector2ui const& newSize)` |
| [`TransientResources`](documentation/generated/Renderer/TransientResources.md)`&` | `Transient()` |
