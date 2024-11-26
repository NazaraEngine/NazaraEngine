---
title: OpenGLFboFramebuffer
description: Nothing
---

# Nz::OpenGLFboFramebuffer

Class description

## Constructors

- `OpenGLFboFramebuffer(`[`OpenGLDevice`](documentation/generated/OpenGLRenderer/OpenGLDevice.md)`& device, std::vector<std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`>> attachments)`
- `OpenGLFboFramebuffer(`OpenGLFboFramebuffer` const&)`
- `OpenGLFboFramebuffer(`OpenGLFboFramebuffer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Activate()` |
| `Nz::Vector2ui` | `GetAttachmentSize(std::size_t i)` |
| `std::size_t` | `GetColorBufferCount()` |
| `Nz::Vector2ui const&` | `GetSize()` |
| `void` | `UpdateDebugName(std::string_view name)` |
| OpenGLFboFramebuffer`&` | `operator=(`OpenGLFboFramebuffer` const&)` |
| OpenGLFboFramebuffer`&` | `operator=(`OpenGLFboFramebuffer`&&)` |
