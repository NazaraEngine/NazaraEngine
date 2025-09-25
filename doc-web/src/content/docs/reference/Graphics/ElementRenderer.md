---
title: ElementRenderer
description: Nothing
---

# Nz::ElementRenderer

Class description

## Constructors

- `ElementRenderer()`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`RenderElementPoolBase`](documentation/generated/Graphics/RenderElementPoolBase.md)`&` | `GetPool()` |
| `std::unique_ptr<`[`ElementRendererData`](documentation/generated/Graphics/ElementRendererData.md)`>` | `InstanciateData()` |
| `void` | `Prepare(`[`ViewerInstance`](documentation/generated/Graphics/ViewerInstance.md)` const& viewerInstance, `[`ElementRendererData`](documentation/generated/Graphics/ElementRendererData.md)`& rendererData, `[`RenderFrame`](documentation/generated/Renderer/RenderFrame.md)`& currentFrame, std::size_t elementCount, Pointer<const `[`RenderElement`](documentation/generated/Graphics/RenderElement.md)`> const* elements, `[`ElementRenderer::RenderStates`](documentation/generated/Graphics/ElementRenderer.RenderStates.md)` const* renderStates)` |
| `void` | `PrepareEnd(`[`RenderFrame`](documentation/generated/Renderer/RenderFrame.md)`& currentFrame, `[`ElementRendererData`](documentation/generated/Graphics/ElementRendererData.md)`& rendererData)` |
| `void` | `Render(`[`ViewerInstance`](documentation/generated/Graphics/ViewerInstance.md)` const& viewerInstance, `[`ElementRendererData`](documentation/generated/Graphics/ElementRendererData.md)`& rendererData, `[`CommandBufferBuilder`](documentation/generated/Renderer/CommandBufferBuilder.md)`& commandBuffer, std::size_t elementCount, Pointer<const `[`RenderElement`](documentation/generated/Graphics/RenderElement.md)`> const* elements)` |
| `void` | `Reset(`[`ElementRendererData`](documentation/generated/Graphics/ElementRendererData.md)`& rendererData, `[`RenderFrame`](documentation/generated/Renderer/RenderFrame.md)`& currentFrame)` |
