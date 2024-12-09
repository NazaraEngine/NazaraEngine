---
title: GL.EGLContextBase
description: Nothing
---

# Nz::GL::EGLContextBase

Class description

## Constructors

- `EGLContextBase(`[`OpenGLDevice`](documentation/generated/OpenGLRenderer/OpenGLDevice.md)` const* device, `[`GL::EGLLoader`](documentation/generated/OpenGLRenderer/GL.EGLLoader.md)` const& loader)`
- `EGLContextBase(`GL::EGLContextBase` const&)`
- `EGLContextBase(`GL::EGLContextBase`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Create(`[`GL::ContextParams`](documentation/generated/OpenGLRenderer/GL.ContextParams.md)` const& params, `GL::EGLContextBase` const* shareContext)` |
| `bool` | `Create(`[`GL::ContextParams`](documentation/generated/OpenGLRenderer/GL.ContextParams.md)` const& params, `[`WindowHandle`](documentation/generated/Platform/WindowHandle.md)` window, `GL::EGLContextBase` const* shareContext)` |
| `void` | `Destroy()` |
| `void` | `EnableVerticalSync(bool enabled)` |
| `bool` | `HasPlatformExtension(std::string const& str)` |
| `void` | `SwapBuffers()` |
| GL::EGLContextBase`&` | `operator=(`GL::EGLContextBase` const&)` |
| GL::EGLContextBase`&` | `operator=(`GL::EGLContextBase`&&)` |
