---
title: GL.Framebuffer
description: Nothing
---

# Nz::GL::Framebuffer

Class description

## Constructors

- `Framebuffer(`GL::Framebuffer` const&)`
- `Framebuffer(`GL::Framebuffer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `GLenum` | `Check()` |
| `void` | `DrawBuffers(GLsizei n, GLenum const* bufs)` |
| `void` | `Renderbuffer(GLenum attachment, GLenum renderbuffer)` |
| `void` | `Texture2D(GLenum attachment, GLenum textarget, GLuint texture, GLint level)` |
| `void` | `TextureLayer(GLenum attachment, GLuint texture, GLint level, GLint layer)` |
| GL::Framebuffer`&` | `operator=(`GL::Framebuffer` const&)` |
| GL::Framebuffer`&` | `operator=(`GL::Framebuffer`&&)` |
