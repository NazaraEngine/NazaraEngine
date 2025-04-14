---
title: RefCounted
description: Nothing
---

# Nz::RefCounted

Class description

## Constructors

- `RefCounted(bool persistent)`
- `RefCounted(`RefCounted` const&)`
- `RefCounted(`RefCounted`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `AddReference()` |
| `unsigned int` | `GetReferenceCount()` |
| `bool` | `IsPersistent()` |
| `bool` | `RemoveReference()` |
| `bool` | `SetPersistent(bool persistent, bool checkReferenceCount)` |
| RefCounted`&` | `operator=(`RefCounted` const&)` |
| RefCounted`&` | `operator=(`RefCounted`&&)` |
