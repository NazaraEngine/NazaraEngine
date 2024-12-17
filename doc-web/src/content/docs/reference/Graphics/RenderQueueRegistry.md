---
title: RenderQueueRegistry
description: Nothing
---

# Nz::RenderQueueRegistry

Class description

## Constructors

- `RenderQueueRegistry()`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Clear()` |
| `std::size_t` | `FetchLayerIndex(int renderLayer)` |
| `std::size_t` | `FetchMaterialInstanceIndex(`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)` const* materialInstance)` |
| `std::size_t` | `FetchPipelineIndex(`[`RenderPipeline`](documentation/generated/Renderer/RenderPipeline.md)` const* pipeline)` |
| `std::size_t` | `FetchSkeletonIndex(`[`Skeleton`](documentation/generated/Utility/Skeleton.md)` const* skeleton)` |
| `std::size_t` | `FetchVertexBuffer(`[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)` const* vertexBuffer)` |
| `std::size_t` | `FetchVertexDeclaration(`[`VertexDeclaration`](documentation/generated/Utility/VertexDeclaration.md)` const* vertexDeclaration)` |
| `void` | `Finalize()` |
| `void` | `RegisterLayer(int renderLayer)` |
| `void` | `RegisterMaterialInstance(`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)` const* materialInstance)` |
| `void` | `RegisterPipeline(`[`RenderPipeline`](documentation/generated/Renderer/RenderPipeline.md)` const* pipeline)` |
| `void` | `RegisterSkeleton(`[`Skeleton`](documentation/generated/Utility/Skeleton.md)` const* skeleton)` |
| `void` | `RegisterVertexBuffer(`[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)` const* vertexBuffer)` |
| `void` | `RegisterVertexDeclaration(`[`VertexDeclaration`](documentation/generated/Utility/VertexDeclaration.md)` const* vertexDeclaration)` |
