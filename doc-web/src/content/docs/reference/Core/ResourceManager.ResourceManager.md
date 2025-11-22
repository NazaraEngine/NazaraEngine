---
title: ResourceManager.ResourceManager
description: Nothing
---

# Nz::ResourceManager::ResourceManager

Class description

## Constructors

- `ResourceManager(Nz::ResourceManager::Loader& loader)`
- `ResourceManager(const ResourceManager<Type, Parameters>&)`
- `ResourceManager(ResourceManager<Type, Parameters>&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Clear()` |
| `std::shared_ptr<Type>` | `Get(std::filesystem::path const& filePath)` |
| `Parameters const&` | `GetDefaultParameters()` |
| `void` | `Register(std::filesystem::path const& filePath, std::shared_ptr<Type> resource)` |
| `void` | `SetDefaultParameters(Parameters params)` |
| `void` | `Unregister(std::filesystem::path const& filePath)` |
| `ResourceManager<Type, Parameters>&` | `operator=(const ResourceManager<Type, Parameters>&)` |
| `ResourceManager<Type, Parameters>&` | `operator=(ResourceManager<Type, Parameters>&&)` |
