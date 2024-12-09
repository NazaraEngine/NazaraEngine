---
title: GL.EGLLoader
description: Nothing
---

# Nz::GL::EGLLoader

Class description

## Constructors

- `EGLLoader(`[`Renderer::Config`](documentation/generated/Renderer/Renderer.Config.md)` const& config)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::shared_ptr<Context>` | `CreateContext(`[`OpenGLDevice`](documentation/generated/OpenGLRenderer/OpenGLDevice.md)` const* device, `[`GL::ContextParams`](documentation/generated/OpenGLRenderer/GL.ContextParams.md)` const& params, `[`GL::Context`](documentation/generated/OpenGLRenderer/GL.Context.md)`* shareContext)` |
| `std::shared_ptr<Context>` | `CreateContext(`[`OpenGLDevice`](documentation/generated/OpenGLRenderer/OpenGLDevice.md)` const* device, `[`GL::ContextParams`](documentation/generated/OpenGLRenderer/GL.ContextParams.md)` const& params, `[`WindowHandle`](documentation/generated/Platform/WindowHandle.md)` handle, `[`GL::Context`](documentation/generated/OpenGLRenderer/GL.Context.md)`* shareContext)` |
| `EGLDisplay` | `GetDefaultDisplay()` |
| `Nz::GL::ContextType` | `GetPreferredContextType()` |
| `Nz::GL::GLFunction` | `LoadFunction(char const* name)` |
