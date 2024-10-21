---
title: ResourceLoader.ResourceLoader
description: Nothing
---

# Nz::ResourceLoader::ResourceLoader

Class description

## Constructors

- `ResourceLoader()`
- `ResourceLoader(const ResourceLoader<Type, Parameters>&)`
- `ResourceLoader(ResourceLoader<Type, Parameters>&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Clear()` |
| `bool` | `IsExtensionSupported(std::string_view const& extension)` |
| `std::shared_ptr<Type>` | `LoadFromFile(std::filesystem::path const& filePath, Parameters const& parameters)` |
| `std::shared_ptr<Type>` | `LoadFromMemory(void const* data, std::size_t size, Parameters const& parameters)` |
| `std::shared_ptr<Type>` | `LoadFromStream(`[`Stream`](documentation/generated/Core/Stream.md)`& stream, Parameters const& parameters)` |
| `Nz::ResourceLoader::Entry const*` | `RegisterLoader(Nz::ResourceLoader::Entry loader)` |
| `void` | `UnregisterLoader(Nz::ResourceLoader::Entry const* loader)` |
| `ResourceLoader<Type, Parameters>&` | `operator=(const ResourceLoader<Type, Parameters>&)` |
| `ResourceLoader<Type, Parameters>&` | `operator=(ResourceLoader<Type, Parameters>&&)` |
