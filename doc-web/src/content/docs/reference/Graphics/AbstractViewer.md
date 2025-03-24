---
title: AbstractViewer
description: Nothing
---

# Nz::AbstractViewer

Class description

## Constructors

- `AbstractViewer()`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`Color`](documentation/generated/Core/Color.md)` const&` | `GetClearColor()` |
| `Nz::UInt32` | `GetRenderMask()` |
| [`RenderTarget`](documentation/generated/Renderer/RenderTarget.md)` const&` | `GetRenderTarget()` |
| [`ViewerInstance`](documentation/generated/Graphics/ViewerInstance.md)`&` | `GetViewerInstance()` |
| [`ViewerInstance`](documentation/generated/Graphics/ViewerInstance.md)` const&` | `GetViewerInstance()` |
| `Nz::Recti const&` | `GetViewport()` |
| `Nz::Vector3f` | `Project(Nz::Vector3f const& worldPos)` |
| `Nz::Vector3f` | `Unproject(Nz::Vector3f const& screenPos)` |
