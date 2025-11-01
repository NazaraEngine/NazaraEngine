---
title: OpenGLTextureSampler
description: Nothing
---

# Nz::OpenGLTextureSampler

Class description

## Constructors

- `OpenGLTextureSampler(`[`OpenGLDevice`](documentation/generated/OpenGLRenderer/OpenGLDevice.md)`& device, `[`TextureSamplerInfo`](documentation/generated/Renderer/TextureSamplerInfo.md)` samplerInfo)`
- `OpenGLTextureSampler(`OpenGLTextureSampler` const&)`
- `OpenGLTextureSampler(`OpenGLTextureSampler`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`GL::Sampler`](documentation/generated/OpenGLRenderer/GL.Sampler.md)` const&` | `GetSampler(bool mipmaps)` |
| `void` | `UpdateDebugName(std::string_view name)` |
| OpenGLTextureSampler`&` | `operator=(`OpenGLTextureSampler` const&)` |
| OpenGLTextureSampler`&` | `operator=(`OpenGLTextureSampler`&&)` |
