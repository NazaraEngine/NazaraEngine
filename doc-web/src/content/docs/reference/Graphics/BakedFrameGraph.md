---
title: BakedFrameGraph
description: Nothing
---

# Nz::BakedFrameGraph

Class description

## Constructors

- `BakedFrameGraph()`
- `BakedFrameGraph(`BakedFrameGraph` const&)`
- `BakedFrameGraph(`BakedFrameGraph`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Execute(`[`RenderFrame`](documentation/generated/Renderer/RenderFrame.md)`& renderFrame)` |
| `std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`> const&` | `GetAttachmentTexture(std::size_t attachmentIndex)` |
| `std::shared_ptr<`[`RenderPass`](documentation/generated/Renderer/RenderPass.md)`> const&` | `GetRenderPass(std::size_t passIndex)` |
| `bool` | `Resize(`[`RenderFrame`](documentation/generated/Renderer/RenderFrame.md)`& renderFrame)` |
| BakedFrameGraph`&` | `operator=(`BakedFrameGraph` const&)` |
| BakedFrameGraph`&` | `operator=(`BakedFrameGraph`&&)` |
