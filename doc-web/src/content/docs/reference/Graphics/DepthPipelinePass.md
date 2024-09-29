---
title: DepthPipelinePass
description: Nothing
---

# Nz::DepthPipelinePass

Class description

## Constructors

- `DepthPipelinePass(`[`FramePipeline`](documentation/generated/Graphics/FramePipeline.md)`& owner, `[`ElementRendererRegistry`](documentation/generated/Graphics/ElementRendererRegistry.md)`& elementRegistry, `[`AbstractViewer`](documentation/generated/Graphics/AbstractViewer.md)`* viewer, std::size_t passIndex, std::string passName)`
- `DepthPipelinePass(`DepthPipelinePass` const&)`
- `DepthPipelinePass(`DepthPipelinePass`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `InvalidateCommandBuffers()` |
| `void` | `InvalidateElements()` |
| `void` | `Prepare(`[`RenderFrame`](documentation/generated/Renderer/RenderFrame.md)`& renderFrame, Nz::Frustumf const& frustum, std::vector<`[`FramePipelinePass::VisibleRenderable`](documentation/generated/Graphics/FramePipelinePass.VisibleRenderable.md)`> const& visibleRenderables, std::size_t visibilityHash)` |
| `void` | `RegisterMaterialInstance(`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)` const& materialInstance)` |
| [`FramePass`](documentation/generated/Graphics/FramePass.md)`&` | `RegisterToFrameGraph(`[`FrameGraph`](documentation/generated/Graphics/FrameGraph.md)`& frameGraph, std::size_t outputAttachment)` |
| `void` | `UnregisterMaterialInstance(`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)` const& materialInstance)` |
| DepthPipelinePass`&` | `operator=(`DepthPipelinePass` const&)` |
| DepthPipelinePass`&` | `operator=(`DepthPipelinePass`&&)` |
