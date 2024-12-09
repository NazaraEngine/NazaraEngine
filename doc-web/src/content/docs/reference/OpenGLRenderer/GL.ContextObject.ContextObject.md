---
title: GL.ContextObject.ContextObject
description: Nothing
---

# Nz::GL::ContextObject::ContextObject

Class description

## Constructors

- `ContextObject()`
- `ContextObject(`[`GL::Context`](documentation/generated/OpenGLRenderer/GL.Context.md)` const& context, CreateArgs... args)`
- `ContextObject(const ContextObject<C, ObjectType, CreateArgs...>&)`
- `ContextObject(ContextObject<C, ObjectType, CreateArgs...>&& object)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Create(`[`GL::Context`](documentation/generated/OpenGLRenderer/GL.Context.md)` const& context, CreateArgs... args)` |
| `void` | `Destroy()` |
| [`GL::Context`](documentation/generated/OpenGLRenderer/GL.Context.md)` const&` | `EnsureContext()` |
| `bool` | `IsValid()` |
| [`GL::Context`](documentation/generated/OpenGLRenderer/GL.Context.md)` const*` | `GetContext()` |
| `GLuint` | `GetObjectId()` |
| `void` | `SetDebugName(std::string_view const& name)` |
| `ContextObject<C, ObjectType, CreateArgs...>&` | `operator=(const ContextObject<C, ObjectType, CreateArgs...>&)` |
| `ContextObject<C, ObjectType, CreateArgs...>&` | `operator=(ContextObject<C, ObjectType, CreateArgs...>&& object)` |
