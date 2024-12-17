---
title: OpenGLShaderBinding
description: Nothing
---

# Nz::OpenGLShaderBinding

Class description

## Constructors

- `OpenGLShaderBinding(`[`OpenGLRenderPipelineLayout`](documentation/generated/OpenGLRenderer/OpenGLRenderPipelineLayout.md)`& owner, std::size_t poolIndex, std::size_t bindingIndex)`
- `OpenGLShaderBinding(`OpenGLShaderBinding` const&)`
- `OpenGLShaderBinding(`OpenGLShaderBinding`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Apply(`[`OpenGLRenderPipelineLayout`](documentation/generated/OpenGLRenderer/OpenGLRenderPipelineLayout.md)` const& pipelineLayout, Nz::UInt32 setIndex, `[`GL::Context`](documentation/generated/OpenGLRenderer/GL.Context.md)` const& context)` |
| `std::size_t` | `GetBindingIndex()` |
| `std::size_t` | `GetPoolIndex()` |
| [`OpenGLRenderPipelineLayout`](documentation/generated/OpenGLRenderer/OpenGLRenderPipelineLayout.md)` const&` | `GetOwner()` |
| `void` | `Update(`[`ShaderBinding::Binding`](documentation/generated/Renderer/ShaderBinding.Binding.md)` const* bindings, std::size_t bindingCount)` |
| `void` | `UpdateDebugName(std::string_view name)` |
| OpenGLShaderBinding`&` | `operator=(`OpenGLShaderBinding` const&)` |
| OpenGLShaderBinding`&` | `operator=(`OpenGLShaderBinding`&&)` |
