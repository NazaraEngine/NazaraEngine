---
title: GL.Context
description: Nothing
---

# Nz::GL::Context

Class description

## Constructors

- `Context(`[`OpenGLDevice`](documentation/generated/OpenGLRenderer/OpenGLDevice.md)` const* device)`
- `Context(`GL::Context` const&)`
- `Context(`GL::Context`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `BindBuffer(Nz::GL::BufferTarget target, GLuint buffer, bool force)` |
| `GLenum` | `BindFramebuffer(GLuint fbo)` |
| `void` | `BindFramebuffer(Nz::GL::FramebufferTarget target, GLuint fbo)` |
| `void` | `BindImageTexture(GLuint imageUnit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format)` |
| `void` | `BindProgram(GLuint program)` |
| `void` | `BindSampler(Nz::UInt32 textureUnit, GLuint sampler)` |
| `void` | `BindStorageBuffer(Nz::UInt32 storageUnit, GLuint buffer, GLintptr offset, GLsizeiptr size)` |
| `void` | `BindTexture(Nz::GL::TextureTarget target, GLuint texture)` |
| `void` | `BindTexture(Nz::UInt32 textureUnit, Nz::GL::TextureTarget target, GLuint texture)` |
| `void` | `BindUniformBuffer(Nz::UInt32 uboUnit, GLuint buffer, GLintptr offset, GLsizeiptr size)` |
| `void` | `BindVertexArray(GLuint vertexArray, bool force)` |
| `bool` | `BlitTexture(`[`OpenGLTexture`](documentation/generated/OpenGLRenderer/OpenGLTexture.md)` const& source, `[`OpenGLTexture`](documentation/generated/OpenGLRenderer/OpenGLTexture.md)` const& destination, Nz::Boxui const& srcBox, Nz::Boxui const& dstBox, Nz::SamplerFilter filter)` |
| `bool` | `ClearErrorStack()` |
| `bool` | `CopyTexture(`[`OpenGLTexture`](documentation/generated/OpenGLRenderer/OpenGLTexture.md)` const& source, `[`OpenGLTexture`](documentation/generated/OpenGLRenderer/OpenGLTexture.md)` const& destination, Nz::Boxui const& srcBox, Nz::Vector3ui const& dstPos)` |
| `bool` | `DidLastCallSucceed()` |
| `void` | `EnableVerticalSync(bool enabled)` |
| `bool` | `GetBoolean(GLenum name)` |
| `bool` | `GetBoolean(GLenum name, GLuint index)` |
| [`OpenGLDevice`](documentation/generated/OpenGLRenderer/OpenGLDevice.md)` const*` | `GetDevice()` |
| `Nz::GL::ExtensionStatus` | `GetExtensionStatus(Nz::GL::Extension extension)` |
| `float` | `GetFloat(GLenum name)` |
| `Nz::GL::GLFunction` | `GetFunctionByIndex(std::size_t funcIndex)` |
| [`GL::ContextParams`](documentation/generated/OpenGLRenderer/GL.ContextParams.md)` const&` | `GetParams()` |
| [`GL::OpenGLVaoCache`](documentation/generated/OpenGLRenderer/GL.OpenGLVaoCache.md)` const&` | `GetVaoCache()` |
| `bool` | `IsExtensionSupported(Nz::GL::Extension extension)` |
| `bool` | `IsExtensionSupported(std::string const& extension)` |
| `bool` | `HasZeroToOneDepth()` |
| `bool` | `Initialize(`[`GL::ContextParams`](documentation/generated/OpenGLRenderer/GL.ContextParams.md)` const& params)` |
| `void` | `NotifyBufferDestruction(GLuint buffer)` |
| `void` | `NotifyFramebufferDestruction(GLuint fbo)` |
| `void` | `NotifyProgramDestruction(GLuint program)` |
| `void` | `NotifySamplerDestruction(GLuint sampler)` |
| `void` | `NotifyTextureDestruction(GLuint texture)` |
| `void` | `NotifyVertexArrayDestruction(GLuint vao)` |
| `bool` | `ProcessErrorStack()` |
| `void` | `ResetColorWriteMasks()` |
| `void` | `ResetDepthWriteMasks()` |
| `void` | `ResetStencilWriteMasks()` |
| `void` | `SetCurrentTextureUnit(Nz::UInt32 textureUnit)` |
| `void` | `SetScissorBox(GLint x, GLint y, GLsizei width, GLsizei height)` |
| `void` | `SetViewport(GLint x, GLint y, GLsizei width, GLsizei height)` |
| `void` | `SwapBuffers()` |
| `void` | `UpdateStates(`[`RenderStates`](documentation/generated/Renderer/RenderStates.md)` const& renderStates, bool isViewportFlipped)` |
| GL::Context`&` | `operator=(`GL::Context` const&)` |
| GL::Context`&` | `operator=(`GL::Context`&&)` |
