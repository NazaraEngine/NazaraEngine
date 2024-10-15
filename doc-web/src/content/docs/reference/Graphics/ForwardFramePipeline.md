---
title: ForwardFramePipeline
description: Nothing
---

# Nz::ForwardFramePipeline

Class description

## Constructors

- `ForwardFramePipeline(`[`ElementRendererRegistry`](documentation/generated/Graphics/ElementRendererRegistry.md)`& elementRegistry)`
- `ForwardFramePipeline(`ForwardFramePipeline` const&)`
- `ForwardFramePipeline(`ForwardFramePipeline`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::vector<`[`FramePipelinePass::VisibleRenderable`](documentation/generated/Graphics/FramePipelinePass.VisibleRenderable.md)`> const&` | `FrustumCull(Nz::Frustumf const& frustum, Nz::UInt32 mask, std::size_t& visibilityHash)` |
| `void` | `ForEachRegisteredMaterialInstance(FunctionRef<void (const `[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)` &)> callback)` |
| `void` | `QueueTransfer(`[`TransferInterface`](documentation/generated/Graphics/TransferInterface.md)`* transfer)` |
| `std::size_t` | `RegisterLight(`[`Light`](documentation/generated/Graphics/Light.md)` const* light, Nz::UInt32 renderMask)` |
| `std::size_t` | `RegisterRenderable(std::size_t worldInstanceIndex, std::size_t skeletonInstanceIndex, `[`InstancedRenderable`](documentation/generated/Graphics/InstancedRenderable.md)` const* instancedRenderable, Nz::UInt32 renderMask, Nz::Recti const& scissorBox)` |
| `std::size_t` | `RegisterSkeleton(Nz::SkeletonInstancePtr skeletonInstance)` |
| `std::size_t` | `RegisterViewer(`[`AbstractViewer`](documentation/generated/Graphics/AbstractViewer.md)`* viewerInstance, Nz::Int32 renderOrder)` |
| `std::size_t` | `RegisterWorldInstance(Nz::WorldInstancePtr worldInstance)` |
| [`Light`](documentation/generated/Graphics/Light.md)` const*` | `RetrieveLight(std::size_t lightIndex)` |
| [`Texture`](documentation/generated/Renderer/Texture.md)` const*` | `RetrieveLightShadowmap(std::size_t lightIndex)` |
| `void` | `Render(`[`RenderFrame`](documentation/generated/Renderer/RenderFrame.md)`& renderFrame)` |
| `void` | `UnregisterLight(std::size_t lightIndex)` |
| `void` | `UnregisterRenderable(std::size_t renderableIndex)` |
| `void` | `UnregisterSkeleton(std::size_t skeletonIndex)` |
| `void` | `UnregisterViewer(std::size_t viewerIndex)` |
| `void` | `UnregisterWorldInstance(std::size_t worldInstance)` |
| `void` | `UpdateLightRenderMask(std::size_t lightIndex, Nz::UInt32 renderMask)` |
| `void` | `UpdateRenderableRenderMask(std::size_t renderableIndex, Nz::UInt32 renderMask)` |
| `void` | `UpdateRenderableScissorBox(std::size_t renderableIndex, Nz::Recti const& scissorBox)` |
| `void` | `UpdateRenderableSkeletonInstance(std::size_t renderableIndex, std::size_t skeletonIndex)` |
| `void` | `UpdateViewerRenderMask(std::size_t viewerIndex, Nz::Int32 renderOrder)` |
| ForwardFramePipeline`&` | `operator=(`ForwardFramePipeline` const&)` |
| ForwardFramePipeline`&` | `operator=(`ForwardFramePipeline`&&)` |
