---
title: GL.Loader
description: Nothing
---

# Nz::GL::Loader

Class description

## Constructors

- `Loader()`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::shared_ptr<Context>` | `CreateContext(`[`OpenGLDevice`](documentation/generated/OpenGLRenderer/OpenGLDevice.md)` const* device, `[`GL::ContextParams`](documentation/generated/OpenGLRenderer/GL.ContextParams.md)` const& params, `[`GL::Context`](documentation/generated/OpenGLRenderer/GL.Context.md)`* shareContext)` |
| `std::shared_ptr<Context>` | `CreateContext(`[`OpenGLDevice`](documentation/generated/OpenGLRenderer/OpenGLDevice.md)` const* device, `[`GL::ContextParams`](documentation/generated/OpenGLRenderer/GL.ContextParams.md)` const& params, `[`WindowHandle`](documentation/generated/Platform/WindowHandle.md)` handle, `[`GL::Context`](documentation/generated/OpenGLRenderer/GL.Context.md)`* shareContext)` |
| `Nz::GL::ContextType` | `GetPreferredContextType()` |
| `Nz::GL::GLFunction` | `LoadFunction(char const* name)` |
