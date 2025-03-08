---
title: OpenGLCommandBuffer
description: Nothing
---

# Nz::OpenGLCommandBuffer

Class description

## Constructors

- `OpenGLCommandBuffer()`
- `OpenGLCommandBuffer(`[`OpenGLCommandPool`](documentation/generated/OpenGLRenderer/OpenGLCommandPool.md)`& owner, std::size_t poolIndex, std::size_t bindingIndex)`
- `OpenGLCommandBuffer(`OpenGLCommandBuffer` const&)`
- `OpenGLCommandBuffer(`OpenGLCommandBuffer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `BeginDebugRegion(std::string_view const& regionName, `[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `BindComputePipeline(`[`OpenGLComputePipeline`](documentation/generated/OpenGLRenderer/OpenGLComputePipeline.md)` const* pipeline)` |
| `void` | `BindComputeShaderBinding(`[`OpenGLRenderPipelineLayout`](documentation/generated/OpenGLRenderer/OpenGLRenderPipelineLayout.md)` const& pipelineLayout, Nz::UInt32 set, `[`OpenGLShaderBinding`](documentation/generated/OpenGLRenderer/OpenGLShaderBinding.md)` const* binding)` |
| `void` | `BindIndexBuffer(GLuint indexBuffer, Nz::IndexType indexType, Nz::UInt64 offset)` |
| `void` | `BindRenderPipeline(`[`OpenGLRenderPipeline`](documentation/generated/OpenGLRenderer/OpenGLRenderPipeline.md)` const* pipeline)` |
| `void` | `BindRenderShaderBinding(`[`OpenGLRenderPipelineLayout`](documentation/generated/OpenGLRenderer/OpenGLRenderPipelineLayout.md)` const& pipelineLayout, Nz::UInt32 set, `[`OpenGLShaderBinding`](documentation/generated/OpenGLRenderer/OpenGLShaderBinding.md)` const* binding)` |
| `void` | `BindVertexBuffer(Nz::UInt32 binding, GLuint vertexBuffer, Nz::UInt64 offset)` |
| `void` | `BlitTexture(`[`OpenGLTexture`](documentation/generated/OpenGLRenderer/OpenGLTexture.md)` const& source, Nz::Boxui const& sourceBox, `[`OpenGLTexture`](documentation/generated/OpenGLRenderer/OpenGLTexture.md)` const& target, Nz::Boxui const& targetBox, Nz::SamplerFilter filter)` |
| `void` | `BuildMipmaps(`[`OpenGLTexture`](documentation/generated/OpenGLRenderer/OpenGLTexture.md)`& texture, Nz::UInt8 baseLevel, Nz::UInt8 levelCount)` |
| `void` | `CopyBuffer(GLuint source, GLuint target, Nz::UInt64 size, Nz::UInt64 sourceOffset, Nz::UInt64 targetOffset)` |
| `void` | `CopyBuffer(`[`UploadPool::Allocation`](documentation/generated/Renderer/UploadPool.Allocation.md)` const& allocation, GLuint target, Nz::UInt64 size, Nz::UInt64 sourceOffset, Nz::UInt64 targetOffset)` |
| `void` | `CopyTexture(`[`OpenGLTexture`](documentation/generated/OpenGLRenderer/OpenGLTexture.md)` const& source, Nz::Boxui const& sourceBox, `[`OpenGLTexture`](documentation/generated/OpenGLRenderer/OpenGLTexture.md)` const& target, Nz::Vector3ui const& targetPoint)` |
| `void` | `Dispatch(Nz::UInt32 numGroupsX, Nz::UInt32 numGroupsY, Nz::UInt32 numGroupsZ)` |
| `void` | `Draw(Nz::UInt32 vertexCount, Nz::UInt32 instanceCount, Nz::UInt32 firstVertex, Nz::UInt32 firstInstance)` |
| `void` | `DrawIndexed(Nz::UInt32 indexCount, Nz::UInt32 instanceCount, Nz::UInt32 firstIndex, Nz::UInt32 firstInstance)` |
| `void` | `EndDebugRegion()` |
| `void` | `Execute()` |
| `std::size_t` | `GetBindingIndex()` |
| `std::size_t` | `GetPoolIndex()` |
| [`OpenGLCommandPool`](documentation/generated/OpenGLRenderer/OpenGLCommandPool.md)` const&` | `GetOwner()` |
| `void` | `InsertMemoryBarrier(GLbitfield barriers)` |
| `void` | `SetFramebuffer(`[`OpenGLFramebuffer`](documentation/generated/OpenGLRenderer/OpenGLFramebuffer.md)` const& framebuffer, `[`OpenGLRenderPass`](documentation/generated/OpenGLRenderer/OpenGLRenderPass.md)` const& renderPass, `[`CommandBufferBuilder::ClearValues`](documentation/generated/Renderer/CommandBufferBuilder.ClearValues.md)` const* clearValues, std::size_t clearValueCount)` |
| `void` | `SetScissor(Nz::Recti const& scissorRegion)` |
| `void` | `SetViewport(Nz::Recti const& viewportRegion)` |
| `void` | `UpdateDebugName(std::string_view name)` |
| OpenGLCommandBuffer`&` | `operator=(`OpenGLCommandBuffer` const&)` |
| OpenGLCommandBuffer`&` | `operator=(`OpenGLCommandBuffer`&&)` |
