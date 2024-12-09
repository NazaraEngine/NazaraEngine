---
title: OpenGLDevice
description: Nothing
---

# Nz::OpenGLDevice

Class description

## Constructors

- `OpenGLDevice(`[`GL::Loader`](documentation/generated/OpenGLRenderer/GL.Loader.md)`& loader, `[`Renderer::Config`](documentation/generated/Renderer/Renderer.Config.md)` const& config)`
- `OpenGLDevice(`OpenGLDevice` const&)`
- `OpenGLDevice(`OpenGLDevice`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::shared_ptr<`[`GL::Context`](documentation/generated/OpenGLRenderer/GL.Context.md)`>` | `CreateContext(`[`GL::ContextParams`](documentation/generated/OpenGLRenderer/GL.ContextParams.md)` params)` |
| `std::shared_ptr<`[`GL::Context`](documentation/generated/OpenGLRenderer/GL.Context.md)`>` | `CreateContext(`[`GL::ContextParams`](documentation/generated/OpenGLRenderer/GL.ContextParams.md)` params, `[`WindowHandle`](documentation/generated/Platform/WindowHandle.md)` handle)` |
| [`RenderDeviceInfo`](documentation/generated/Renderer/RenderDeviceInfo.md)` const&` | `GetDeviceInfo()` |
| [`RenderDeviceFeatures`](documentation/generated/Renderer/RenderDeviceFeatures.md)` const&` | `GetEnabledFeatures()` |
| [`GL::Context`](documentation/generated/OpenGLRenderer/GL.Context.md)` const&` | `GetReferenceContext()` |
| `std::shared_ptr<`[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)`>` | `InstantiateBuffer(Nz::BufferType type, Nz::UInt64 size, Nz::BufferUsageFlags usageFlags, void const* initialData)` |
| `std::shared_ptr<`[`CommandPool`](documentation/generated/Renderer/CommandPool.md)`>` | `InstantiateCommandPool(Nz::QueueType queueType)` |
| `std::shared_ptr<`[`ComputePipeline`](documentation/generated/Renderer/ComputePipeline.md)`>` | `InstantiateComputePipeline(`[`ComputePipelineInfo`](documentation/generated/Renderer/ComputePipelineInfo.md)` pipelineInfo)` |
| `std::shared_ptr<`[`Framebuffer`](documentation/generated/Renderer/Framebuffer.md)`>` | `InstantiateFramebuffer(unsigned int width, unsigned int height, std::shared_ptr<`[`RenderPass`](documentation/generated/Renderer/RenderPass.md)`> const& renderPass, std::vector<std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`>> const& attachments)` |
| `std::shared_ptr<`[`RenderPass`](documentation/generated/Renderer/RenderPass.md)`>` | `InstantiateRenderPass(std::vector<`[`RenderPass::Attachment`](documentation/generated/Renderer/RenderPass.Attachment.md)`> attachments, std::vector<`[`RenderPass::SubpassDescription`](documentation/generated/Renderer/RenderPass.SubpassDescription.md)`> subpassDescriptions, std::vector<`[`RenderPass::SubpassDependency`](documentation/generated/Renderer/RenderPass.SubpassDependency.md)`> subpassDependencies)` |
| `std::shared_ptr<`[`RenderPipeline`](documentation/generated/Renderer/RenderPipeline.md)`>` | `InstantiateRenderPipeline(`[`RenderPipelineInfo`](documentation/generated/Renderer/RenderPipelineInfo.md)` pipelineInfo)` |
| `std::shared_ptr<`[`RenderPipelineLayout`](documentation/generated/Renderer/RenderPipelineLayout.md)`>` | `InstantiateRenderPipelineLayout(`[`RenderPipelineLayoutInfo`](documentation/generated/Renderer/RenderPipelineLayoutInfo.md)` pipelineLayoutInfo)` |
| `std::shared_ptr<`[`ShaderModule`](documentation/generated/Renderer/ShaderModule.md)`>` | `InstantiateShaderModule(nzsl::ShaderStageTypeFlags shaderStages, nzsl::Ast::Module const& shaderModule, nzsl::ShaderWriter::States const& states)` |
| `std::shared_ptr<`[`ShaderModule`](documentation/generated/Renderer/ShaderModule.md)`>` | `InstantiateShaderModule(nzsl::ShaderStageTypeFlags shaderStages, Nz::ShaderLanguage lang, void const* source, std::size_t sourceSize, nzsl::ShaderWriter::States const& states)` |
| `std::shared_ptr<`[`Swapchain`](documentation/generated/Renderer/Swapchain.md)`>` | `InstantiateSwapchain(`[`WindowHandle`](documentation/generated/Platform/WindowHandle.md)` windowHandle, Nz::Vector2ui const& windowSize, `[`SwapchainParameters`](documentation/generated/Renderer/SwapchainParameters.md)` const& parameters)` |
| `std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`>` | `InstantiateTexture(`[`TextureInfo`](documentation/generated/Renderer/TextureInfo.md)` const& params)` |
| `std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`>` | `InstantiateTexture(`[`TextureInfo`](documentation/generated/Renderer/TextureInfo.md)` const& params, void const* initialData, bool buildMipmaps, unsigned int srcWidth, unsigned int srcHeight)` |
| `std::shared_ptr<`[`TextureSampler`](documentation/generated/Renderer/TextureSampler.md)`>` | `InstantiateTextureSampler(`[`TextureSamplerInfo`](documentation/generated/Renderer/TextureSamplerInfo.md)` const& params)` |
| `bool` | `IsTextureFormatSupported(Nz::PixelFormat format, Nz::TextureUsage usage)` |
| `void` | `NotifyBufferDestruction(GLuint buffer)` |
| `void` | `NotifyProgramDestruction(GLuint program)` |
| `void` | `NotifySamplerDestruction(GLuint sampler)` |
| `void` | `NotifyTextureDestruction(GLuint texture)` |
| `void` | `WaitForIdle()` |
| OpenGLDevice`&` | `operator=(`OpenGLDevice` const&)` |
| OpenGLDevice`&` | `operator=(`OpenGLDevice`&&)` |
