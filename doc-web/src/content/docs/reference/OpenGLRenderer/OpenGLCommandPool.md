---
title: OpenGLCommandPool
description: Nothing
---

# Nz::OpenGLCommandPool

Class description

## Constructors

- `OpenGLCommandPool()`
- `OpenGLCommandPool(`OpenGLCommandPool` const&)`
- `OpenGLCommandPool(`OpenGLCommandPool`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::CommandBufferPtr` | `BuildCommandBuffer(std::function<void (CommandBufferBuilder &)> const& callback)` |
| `void` | `UpdateDebugName(std::string_view name)` |
| OpenGLCommandPool`&` | `operator=(`OpenGLCommandPool` const&)` |
| OpenGLCommandPool`&` | `operator=(`OpenGLCommandPool`&&)` |
