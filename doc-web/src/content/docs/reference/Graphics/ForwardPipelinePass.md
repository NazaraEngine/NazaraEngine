---
title: ForwardPipelinePass
description: Nothing
---

# Nz::ForwardPipelinePass

Class description

## Constructors

- `ForwardPipelinePass(`[`FramePipeline`](documentation/generated/Graphics/FramePipeline.md)`& owner, `[`ElementRendererRegistry`](documentation/generated/Graphics/ElementRendererRegistry.md)`& elementRegistry, `[`AbstractViewer`](documentation/generated/Graphics/AbstractViewer.md)`* viewer)`
- `ForwardPipelinePass(`ForwardPipelinePass` const&)`
- `ForwardPipelinePass(`ForwardPipelinePass`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `InvalidateCommandBuffers()` |
| `void` | `InvalidateElements()` |
| `void` | `Prepare(`[`RenderFrame`](documentation/generated/Renderer/RenderFrame.md)`& renderFrame, Nz::Frustumf const& frustum, std::vector<`[`FramePipelinePass::VisibleRenderable`](documentation/generated/Graphics/FramePipelinePass.VisibleRenderable.md)`> const& visibleRenderables, std::vector<std::size_t> const& visibleLights, std::size_t visibilityHash)` |
| `void` | `RegisterMaterialInstance(`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)` const& material)` |
| [`FramePass`](documentation/generated/Graphics/FramePass.md)`&` | `RegisterToFrameGraph(`[`FrameGraph`](documentation/generated/Graphics/FrameGraph.md)`& frameGraph, std::size_t colorBufferIndex, std::size_t depthBufferIndex, bool hasDepthPrepass)` |
| `void` | `UnregisterMaterialInstance(`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)` const& material)` |
| ForwardPipelinePass`&` | `operator=(`ForwardPipelinePass` const&)` |
| ForwardPipelinePass`&` | `operator=(`ForwardPipelinePass`&&)` |
