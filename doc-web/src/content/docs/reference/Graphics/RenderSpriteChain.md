---
title: RenderSpriteChain
description: Nothing
---

# Nz::RenderSpriteChain

Class description

## Constructors

- `RenderSpriteChain(int renderLayer, std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> materialInstance, Nz::MaterialPassFlags materialFlags, std::shared_ptr<`[`RenderPipeline`](documentation/generated/Renderer/RenderPipeline.md)`> renderPipeline, `[`WorldInstance`](documentation/generated/Graphics/WorldInstance.md)` const& worldInstance, std::shared_ptr<`[`VertexDeclaration`](documentation/generated/Utility/VertexDeclaration.md)`> vertexDeclaration, std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`> textureOverlay, std::size_t spriteCount, void const* spriteData, Nz::Recti const& scissorBox)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::UInt64` | `ComputeSortingScore(Nz::Frustumf const& frustum, `[`RenderQueueRegistry`](documentation/generated/Graphics/RenderQueueRegistry.md)` const& registry)` |
| [`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)` const&` | `GetMaterialInstance()` |
| [`RenderPipeline`](documentation/generated/Renderer/RenderPipeline.md)` const&` | `GetRenderPipeline()` |
| `Nz::Recti const&` | `GetScissorBox()` |
| `std::size_t` | `GetSpriteCount()` |
| `void const*` | `GetSpriteData()` |
| [`Texture`](documentation/generated/Renderer/Texture.md)` const*` | `GetTextureOverlay()` |
| [`VertexDeclaration`](documentation/generated/Utility/VertexDeclaration.md)` const*` | `GetVertexDeclaration()` |
| [`WorldInstance`](documentation/generated/Graphics/WorldInstance.md)` const&` | `GetWorldInstance()` |
| `void` | `Register(`[`RenderQueueRegistry`](documentation/generated/Graphics/RenderQueueRegistry.md)`& registry)` |
