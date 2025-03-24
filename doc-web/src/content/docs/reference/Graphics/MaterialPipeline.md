---
title: MaterialPipeline
description: Nothing
---

# Nz::MaterialPipeline

Class description

## Constructors

- `MaterialPipeline(`[`MaterialPipelineInfo`](documentation/generated/Graphics/MaterialPipelineInfo.md)` const& pipelineInfo, `[`MaterialPipeline::Token`](documentation/generated/Graphics/MaterialPipeline.Token.md)`)`
- `MaterialPipeline(`MaterialPipeline` const&)`
- `MaterialPipeline(`MaterialPipeline`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| MaterialPipeline`&` | `operator=(`MaterialPipeline` const&)` |
| MaterialPipeline`&` | `operator=(`MaterialPipeline`&&)` |
| [`MaterialPipelineInfo`](documentation/generated/Graphics/MaterialPipelineInfo.md)` const&` | `GetInfo()` |
| `std::shared_ptr<`[`RenderPipeline`](documentation/generated/Renderer/RenderPipeline.md)`> const&` | `GetRenderPipeline(`[`RenderPipelineInfo::VertexBufferData`](documentation/generated/Renderer/RenderPipelineInfo.VertexBufferData.md)` const* vertexBuffers, std::size_t vertexBufferCount)` |
