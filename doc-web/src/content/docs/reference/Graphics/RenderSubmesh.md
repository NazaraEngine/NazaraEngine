---
title: RenderSubmesh
description: Nothing
---

# Nz::RenderSubmesh

Class description

## Constructors

- `RenderSubmesh(int renderLayer, std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> materialInstance, Nz::MaterialPassFlags materialFlags, std::shared_ptr<`[`RenderPipeline`](documentation/generated/Renderer/RenderPipeline.md)`> renderPipeline, `[`WorldInstance`](documentation/generated/Graphics/WorldInstance.md)` const& worldInstance, `[`SkeletonInstance`](documentation/generated/Graphics/SkeletonInstance.md)` const* skeletonInstance, std::size_t indexCount, Nz::IndexType indexType, std::shared_ptr<`[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)`> indexBuffer, std::shared_ptr<`[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)`> vertexBuffer, Nz::Recti const& scissorBox)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::UInt64` | `ComputeSortingScore(Nz::Frustumf const& frustum, `[`RenderQueueRegistry`](documentation/generated/Graphics/RenderQueueRegistry.md)` const& registry)` |
| [`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)` const*` | `GetIndexBuffer()` |
| `std::size_t` | `GetIndexCount()` |
| `Nz::IndexType` | `GetIndexType()` |
| [`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)` const&` | `GetMaterialInstance()` |
| [`RenderPipeline`](documentation/generated/Renderer/RenderPipeline.md)` const*` | `GetRenderPipeline()` |
| `Nz::Recti const&` | `GetScissorBox()` |
| [`SkeletonInstance`](documentation/generated/Graphics/SkeletonInstance.md)` const*` | `GetSkeletonInstance()` |
| [`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)` const*` | `GetVertexBuffer()` |
| [`WorldInstance`](documentation/generated/Graphics/WorldInstance.md)` const&` | `GetWorldInstance()` |
| `void` | `Register(`[`RenderQueueRegistry`](documentation/generated/Graphics/RenderQueueRegistry.md)`& registry)` |
