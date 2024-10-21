---
title: Plugin.Plugin
description: Nothing
---

# Nz::Plugin::Plugin

Class description

## Constructors

- `Plugin(`[`DynLib`](documentation/generated/Core/DynLib.md)` dynLib, std::unique_ptr<T> pluginInterface, bool isActive)`
- `Plugin(const Plugin<T>&)`
- `Plugin(Plugin<T>&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Activate()` |
| `void` | `Deactivate()` |
| [`DynLib`](documentation/generated/Core/DynLib.md)` const&` | `GetDynamicLibrary()` |
| `T*` | `operator->()` |
| `T const*` | `operator->()` |
| `Plugin<T>&` | `operator=(const Plugin<T>&)` |
| `Plugin<T>&` | `operator=(Plugin<T>&&)` |
