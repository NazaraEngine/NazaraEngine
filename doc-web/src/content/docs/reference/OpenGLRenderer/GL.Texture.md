---
title: GL.Texture
description: Nothing
---

# Nz::GL::Texture

Class description

## Constructors

- `Texture(`GL::Texture` const&)`
- `Texture(`GL::Texture`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `GenerateMipmap()` |
| `Nz::GL::TextureTarget` | `GetTarget()` |
| `void` | `SetParameterf(GLenum pname, GLfloat param)` |
| `void` | `SetParameteri(GLenum pname, GLint param)` |
| `void` | `SetParameterfv(GLenum pname, GLfloat const* param)` |
| `void` | `SetParameteriv(GLenum pname, GLint const* param)` |
| `void` | `TexImage2D(Nz::GL::TextureTarget target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type)` |
| `void` | `TexImage2D(Nz::GL::TextureTarget target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, void const* data)` |
| `void` | `TexImage3D(Nz::GL::TextureTarget target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type)` |
| `void` | `TexImage3D(Nz::GL::TextureTarget target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, void const* data)` |
| `void` | `TexStorage2D(Nz::GL::TextureTarget target, GLint levels, GLint internalFormat, GLsizei width, GLsizei height)` |
| `void` | `TexStorage3D(Nz::GL::TextureTarget target, GLint levels, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth)` |
| `void` | `TexSubImage2D(Nz::GL::TextureTarget target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, void const* data)` |
| `void` | `TexSubImage3D(Nz::GL::TextureTarget target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, void const* data)` |
| `void` | `TextureView(Nz::GL::TextureTarget target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers)` |
| GL::Texture`&` | `operator=(`GL::Texture` const&)` |
| GL::Texture`&` | `operator=(`GL::Texture`&&)` |
