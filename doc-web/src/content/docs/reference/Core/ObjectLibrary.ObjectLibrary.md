---
title: ObjectLibrary.ObjectLibrary
description: Nothing
---

# Nz::ObjectLibrary::ObjectLibrary

Class description

## Constructors

- `ObjectLibrary()`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Clear()` |
| `std::shared_ptr<Type>` | `Get(std::string const& name)` |
| `bool` | `Has(std::string const& name)` |
| `void` | `Register(std::string const& name, std::shared_ptr<Type> object)` |
| `std::shared_ptr<Type>` | `Query(std::string const& name)` |
| `void` | `Unregister(std::string const& name)` |
