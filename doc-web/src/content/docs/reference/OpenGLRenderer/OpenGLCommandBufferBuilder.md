---
title: OpenGLCommandBufferBuilder
description: Nothing
---

# Nz::OpenGLCommandBufferBuilder

Class description

## Constructors

- `OpenGLCommandBufferBuilder(`[`OpenGLCommandBuffer`](documentation/generated/OpenGLRenderer/OpenGLCommandBuffer.md)`& commandBuffer)`
- `OpenGLCommandBufferBuilder(`OpenGLCommandBufferBuilder` const&)`
- `OpenGLCommandBufferBuilder(`OpenGLCommandBufferBuilder`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `BeginDebugRegion(std::string_view const& regionName, `[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `BeginRenderPass(`[`Framebuffer`](documentation/generated/Renderer/Framebuffer.md)` const& framebuffer, `[`RenderPass`](documentation/generated/Renderer/RenderPass.md)` const& renderPass, Nz::Recti const& renderRect, `[`CommandBufferBuilder::ClearValues`](documentation/generated/Renderer/CommandBufferBuilder.ClearValues.md)` const* clearValues, std::size_t clearValueCount)` |
| `void` | `BindComputePipeline(`[`ComputePipeline`](documentation/generated/Renderer/ComputePipeline.md)` const& pipeline)` |
| `void` | `BindComputeShaderBinding(Nz::UInt32 set, `[`ShaderBinding`](documentation/generated/Renderer/ShaderBinding.md)` const& binding)` |
| `void` | `BindComputeShaderBinding(`[`RenderPipelineLayout`](documentation/generated/Renderer/RenderPipelineLayout.md)` const& pipelineLayout, Nz::UInt32 set, `[`ShaderBinding`](documentation/generated/Renderer/ShaderBinding.md)` const& binding)` |
| `void` | `BindIndexBuffer(`[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)` const& indexBuffer, Nz::IndexType indexType, Nz::UInt64 offset)` |
| `void` | `BindRenderPipeline(`[`RenderPipeline`](documentation/generated/Renderer/RenderPipeline.md)` const& pipeline)` |
| `void` | `BindRenderShaderBinding(Nz::UInt32 set, `[`ShaderBinding`](documentation/generated/Renderer/ShaderBinding.md)` const& binding)` |
| `void` | `BindRenderShaderBinding(`[`RenderPipelineLayout`](documentation/generated/Renderer/RenderPipelineLayout.md)` const& pipelineLayout, Nz::UInt32 set, `[`ShaderBinding`](documentation/generated/Renderer/ShaderBinding.md)` const& binding)` |
| `void` | `BindVertexBuffer(Nz::UInt32 binding, `[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)` const& vertexBuffer, Nz::UInt64 offset)` |
| `void` | `BlitTexture(`[`Texture`](documentation/generated/Renderer/Texture.md)` const& fromTexture, Nz::Boxui const& fromBox, Nz::TextureLayout fromLayout, `[`Texture`](documentation/generated/Renderer/Texture.md)` const& toTexture, Nz::Boxui const& toBox, Nz::TextureLayout toLayout, Nz::SamplerFilter filter)` |
| `void` | `BuildMipmaps(`[`Texture`](documentation/generated/Renderer/Texture.md)`& texture, Nz::UInt8 baseLevel, Nz::UInt8 levelCount, Nz::PipelineStageFlags srcStageMask, Nz::PipelineStageFlags dstStageMask, Nz::MemoryAccessFlags srcAccessMask, Nz::MemoryAccessFlags dstAccessMask, Nz::TextureLayout oldLayout, Nz::TextureLayout newLayout)` |
| `void` | `CopyBuffer(`[`RenderBufferView`](documentation/generated/Renderer/RenderBufferView.md)` const& source, `[`RenderBufferView`](documentation/generated/Renderer/RenderBufferView.md)` const& target, Nz::UInt64 size, Nz::UInt64 sourceOffset, Nz::UInt64 targetOffset)` |
| `void` | `CopyBuffer(`[`UploadPool::Allocation`](documentation/generated/Renderer/UploadPool.Allocation.md)` const& allocation, `[`RenderBufferView`](documentation/generated/Renderer/RenderBufferView.md)` const& target, Nz::UInt64 size, Nz::UInt64 sourceOffset, Nz::UInt64 targetOffset)` |
| `void` | `CopyTexture(`[`Texture`](documentation/generated/Renderer/Texture.md)` const& fromTexture, Nz::Boxui const& fromBox, Nz::TextureLayout fromLayout, `[`Texture`](documentation/generated/Renderer/Texture.md)` const& toTexture, Nz::Vector3ui const& toPos, Nz::TextureLayout toLayout)` |
| `void` | `Dispatch(Nz::UInt32 workgroupX, Nz::UInt32 workgroupY, Nz::UInt32 workgroupZ)` |
| `void` | `Draw(Nz::UInt32 vertexCount, Nz::UInt32 instanceCount, Nz::UInt32 firstVertex, Nz::UInt32 firstInstance)` |
| `void` | `DrawIndexed(Nz::UInt32 indexCount, Nz::UInt32 instanceCount, Nz::UInt32 firstIndex, Nz::UInt32 firstInstance)` |
| `void` | `EndDebugRegion()` |
| `void` | `EndRenderPass()` |
| `void` | `NextSubpass()` |
| `void` | `PreTransferBarrier()` |
| `void` | `PostTransferBarrier()` |
| `void` | `SetScissor(Nz::Recti const& scissorRegion)` |
| `void` | `SetViewport(Nz::Recti const& viewportRegion)` |
| `void` | `TextureBarrier(Nz::PipelineStageFlags srcStageMask, Nz::PipelineStageFlags dstStageMask, Nz::MemoryAccessFlags srcAccessMask, Nz::MemoryAccessFlags dstAccessMask, Nz::TextureLayout oldLayout, Nz::TextureLayout newLayout, `[`Texture`](documentation/generated/Renderer/Texture.md)` const& texture)` |
| OpenGLCommandBufferBuilder`&` | `operator=(`OpenGLCommandBufferBuilder` const&)` |
| OpenGLCommandBufferBuilder`&` | `operator=(`OpenGLCommandBufferBuilder`&&)` |
