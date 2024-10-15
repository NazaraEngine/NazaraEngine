---
title: DynLib
description: Nothing
---

# Nz::DynLib

Class description

## Constructors

- `DynLib()`
- `DynLib(`DynLib` const&)`
- `DynLib(`DynLib`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::string` | `GetLastError()` |
| `Nz::DynLibFunc` | `GetSymbol(char const* symbol)` |
| `bool` | `IsLoaded()` |
| `bool` | `Load(std::filesystem::path libraryPath)` |
| `void` | `Unload()` |
| DynLib`&` | `operator=(`DynLib` const&)` |
| DynLib`&` | `operator=(`DynLib`&& lib)` |
