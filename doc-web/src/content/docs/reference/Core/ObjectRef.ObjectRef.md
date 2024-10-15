---
title: ObjectRef.ObjectRef
description: Nothing
---

# Nz::ObjectRef::ObjectRef

Class description

## Constructors

- `ObjectRef()`
- `ObjectRef(T* object)`
- `ObjectRef(const ObjectRef<T>& ref)`
- `ObjectRef(ObjectRef<T>&& ref)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `T*` | `Get()` |
| `bool` | `IsValid()` |
| `T*` | `Release()` |
| `bool` | `Reset(T* object)` |
| `ObjectRef<T>&` | `Swap(ObjectRef<T>& ref)` |
| `T*` | `operator->()` |
| `ObjectRef<T>&` | `operator=(T* object)` |
| `ObjectRef<T>&` | `operator=(const ObjectRef<T>& ref)` |
| `ObjectRef<T>&` | `operator=(ObjectRef<T>&& ref)` |
