---
title: VirtualDirectoryResolver
description: Nothing
---

# Nz::VirtualDirectoryResolver

Class description

## Constructors

- `VirtualDirectoryResolver()`
- `VirtualDirectoryResolver(`VirtualDirectoryResolver` const&)`
- `VirtualDirectoryResolver(`VirtualDirectoryResolver`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `ForEach(std::weak_ptr<`[`VirtualDirectory`](documentation/generated/Core/VirtualDirectory.md)`> parent, FunctionRef<bool (std::string_view, VirtualDirectory::Entry &&)> callback)` |
| `std::optional<VirtualDirectory::Entry>` | `Resolve(std::weak_ptr<`[`VirtualDirectory`](documentation/generated/Core/VirtualDirectory.md)`> parent, std::string_view const* parts, std::size_t partCount)` |
| VirtualDirectoryResolver`&` | `operator=(`VirtualDirectoryResolver` const&)` |
| VirtualDirectoryResolver`&` | `operator=(`VirtualDirectoryResolver`&&)` |
