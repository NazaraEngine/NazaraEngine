---
title: VirtualDirectoryFilesystemResolver
description: Nothing
---

# Nz::VirtualDirectoryFilesystemResolver

Class description

## Constructors

- `VirtualDirectoryFilesystemResolver(std::filesystem::path physicalPath, Nz::OpenModeFlags fileOpenMode)`
- `VirtualDirectoryFilesystemResolver(`VirtualDirectoryFilesystemResolver` const&)`
- `VirtualDirectoryFilesystemResolver(`VirtualDirectoryFilesystemResolver`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `ForEach(std::weak_ptr<`[`VirtualDirectory`](documentation/generated/Core/VirtualDirectory.md)`> parent, FunctionRef<bool (std::string_view, VirtualDirectory::Entry &&)> callback)` |
| `std::optional<VirtualDirectory::Entry>` | `Resolve(std::weak_ptr<`[`VirtualDirectory`](documentation/generated/Core/VirtualDirectory.md)`> parent, std::string_view const* parts, std::size_t partCount)` |
| VirtualDirectoryFilesystemResolver`&` | `operator=(`VirtualDirectoryFilesystemResolver` const&)` |
| VirtualDirectoryFilesystemResolver`&` | `operator=(`VirtualDirectoryFilesystemResolver`&&)` |
