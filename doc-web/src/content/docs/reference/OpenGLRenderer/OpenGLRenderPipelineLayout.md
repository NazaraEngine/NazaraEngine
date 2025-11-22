---
title: OpenGLRenderPipelineLayout
description: Nothing
---

# Nz::OpenGLRenderPipelineLayout

Class description

## Constructors

- `OpenGLRenderPipelineLayout(`[`RenderPipelineLayoutInfo`](documentation/generated/Renderer/RenderPipelineLayoutInfo.md)` layoutInfo)`
- `OpenGLRenderPipelineLayout(`OpenGLRenderPipelineLayout` const&)`
- `OpenGLRenderPipelineLayout(`OpenGLRenderPipelineLayout`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::ShaderBindingPtr` | `AllocateShaderBinding(Nz::UInt32 setIndex)` |
| `nzsl::GlslWriter::BindingMapping const&` | `GetBindingMapping()` |
| [`RenderPipelineLayoutInfo`](documentation/generated/Renderer/RenderPipelineLayoutInfo.md)` const&` | `GetLayoutInfo()` |
| `void` | `UpdateDebugName(std::string_view name)` |
| OpenGLRenderPipelineLayout`&` | `operator=(`OpenGLRenderPipelineLayout` const&)` |
| OpenGLRenderPipelineLayout`&` | `operator=(`OpenGLRenderPipelineLayout`&&)` |
