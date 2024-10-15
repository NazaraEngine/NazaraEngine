---
title: ObjectHandle.ObjectHandle
description: Nothing
---

# Nz::ObjectHandle::ObjectHandle

Class description

## Constructors

- `ObjectHandle()`
- `ObjectHandle(T* object)`
- `ObjectHandle(const ObjectHandle<T>& handle)`
- `ObjectHandle(ObjectHandle<T>&& handle)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `T*` | `GetObject()` |
| `bool` | `IsValid()` |
| `void` | `Reset(T* object)` |
| `void` | `Reset(const ObjectHandle<T>& handle)` |
| `void` | `Reset(ObjectHandle<T>&& handle)` |
| `ObjectHandle<T>&` | `Swap(ObjectHandle<T>& handle)` |
| `std::string` | `ToString()` |
| `T*` | `operator->()` |
| `ObjectHandle<T>&` | `operator=(T* object)` |
| `ObjectHandle<T>&` | `operator=(const ObjectHandle<T>& handle)` |
| `ObjectHandle<T>&` | `operator=(ObjectHandle<T>&& handle)` |
