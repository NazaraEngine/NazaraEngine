---
title: PluginLoader
description: Nothing
---

# Nz::PluginLoader

Class description

## Constructors

- `PluginLoader()`
- `PluginLoader(`PluginLoader` const&)`
- `PluginLoader(`PluginLoader`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `AddSearchDirectory(std::filesystem::path const& directoryPath)` |
| `Nz::GenericPlugin` | `Load(std::filesystem::path const& pluginPath, bool activate)` |
| `void` | `RemoveSearchDirectory(std::filesystem::path const& directoryPath)` |
| PluginLoader`&` | `operator=(`PluginLoader` const&)` |
| PluginLoader`&` | `operator=(`PluginLoader`&&)` |
