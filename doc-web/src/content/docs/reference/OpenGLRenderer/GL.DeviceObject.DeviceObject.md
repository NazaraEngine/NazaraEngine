---
title: GL.DeviceObject.DeviceObject
description: Nothing
---

# Nz::GL::DeviceObject::DeviceObject

Class description

## Constructors

- `DeviceObject()`
- `DeviceObject(`[`OpenGLDevice`](documentation/generated/OpenGLRenderer/OpenGLDevice.md)`& device, CreateArgs... args)`
- `DeviceObject(const DeviceObject<C, ObjectType, CreateArgs...>&)`
- `DeviceObject(DeviceObject<C, ObjectType, CreateArgs...>&& object)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Create(`[`OpenGLDevice`](documentation/generated/OpenGLRenderer/OpenGLDevice.md)`& device, CreateArgs... args)` |
| `void` | `Destroy()` |
| [`GL::Context`](documentation/generated/OpenGLRenderer/GL.Context.md)` const&` | `EnsureDeviceContext()` |
| `bool` | `IsValid()` |
| [`OpenGLDevice`](documentation/generated/OpenGLRenderer/OpenGLDevice.md)`*` | `GetDevice()` |
| `GLuint` | `GetObjectId()` |
| `void` | `SetDebugName(std::string_view const& name)` |
| `DeviceObject<C, ObjectType, CreateArgs...>&` | `operator=(const DeviceObject<C, ObjectType, CreateArgs...>&)` |
| `DeviceObject<C, ObjectType, CreateArgs...>&` | `operator=(DeviceObject<C, ObjectType, CreateArgs...>&& object)` |
