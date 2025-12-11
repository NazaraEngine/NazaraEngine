---
title: ResourceSaver.ResourceSaver
description: Nothing
---

# Nz::ResourceSaver::ResourceSaver

Class description

## Constructors

- `ResourceSaver()`
- `ResourceSaver(const ResourceSaver<Type, Parameters>&)`
- `ResourceSaver(ResourceSaver<Type, Parameters>&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Clear()` |
| `bool` | `IsExtensionSupported(std::string_view const& extension)` |
| `bool` | `SaveToFile(Type const& resource, std::filesystem::path const& filePath, Parameters const& parameters)` |
| `bool` | `SaveToStream(Type const& resource, `[`Stream`](documentation/generated/Core/Stream.md)`& stream, std::string const& format, Parameters const& parameters)` |
| `Nz::ResourceSaver::Entry const*` | `RegisterSaver(Nz::ResourceSaver::Entry saver)` |
| `void` | `UnregisterSaver(Nz::ResourceSaver::Entry const* saver)` |
| `ResourceSaver<Type, Parameters>&` | `operator=(const ResourceSaver<Type, Parameters>&)` |
| `ResourceSaver<Type, Parameters>&` | `operator=(ResourceSaver<Type, Parameters>&&)` |
