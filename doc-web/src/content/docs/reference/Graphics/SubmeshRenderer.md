---
title: SubmeshRenderer
description: Nothing
---

# Nz::SubmeshRenderer

Class description

## Constructors

- `SubmeshRenderer()`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `RenderElementPool<`[`RenderSubmesh`](documentation/generated/Graphics/RenderSubmesh.md)`>&` | `GetPool()` |
| `std::unique_ptr<`[`ElementRendererData`](documentation/generated/Graphics/ElementRendererData.md)`>` | `InstanciateData()` |
| `void` | `Prepare(`[`ViewerInstance`](documentation/generated/Graphics/ViewerInstance.md)` const& viewerInstance, `[`ElementRendererData`](documentation/generated/Graphics/ElementRendererData.md)`& rendererData, `[`RenderFrame`](documentation/generated/Renderer/RenderFrame.md)`& currentFrame, std::size_t elementCount, Pointer<const `[`RenderElement`](documentation/generated/Graphics/RenderElement.md)`> const* elements, `[`ElementRenderer::RenderStates`](documentation/generated/Graphics/ElementRenderer.RenderStates.md)` const* renderStates)` |
| `void` | `Render(`[`ViewerInstance`](documentation/generated/Graphics/ViewerInstance.md)` const& viewerInstance, `[`ElementRendererData`](documentation/generated/Graphics/ElementRendererData.md)`& rendererData, `[`CommandBufferBuilder`](documentation/generated/Renderer/CommandBufferBuilder.md)`& commandBuffer, std::size_t elementCount, Pointer<const `[`RenderElement`](documentation/generated/Graphics/RenderElement.md)`> const* elements)` |
| `void` | `Reset(`[`ElementRendererData`](documentation/generated/Graphics/ElementRendererData.md)`& rendererData, `[`RenderFrame`](documentation/generated/Renderer/RenderFrame.md)`& currentFrame)` |
