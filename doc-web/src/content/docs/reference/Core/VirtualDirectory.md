---
title: VirtualDirectory
description: Nothing
---

# Nz::VirtualDirectory

Class description

## Constructors

- `VirtualDirectory(std::weak_ptr<`VirtualDirectory`> parentDirectory)`
- `VirtualDirectory(std::shared_ptr<`[`VirtualDirectoryResolver`](documentation/generated/Core/VirtualDirectoryResolver.md)`> resolver, std::weak_ptr<`VirtualDirectory`> parentDirectory)`
- `VirtualDirectory(`VirtualDirectory` const&)`
- `VirtualDirectory(`VirtualDirectory`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `AllowUproot(bool uproot)` |
| `bool` | `Exists(std::string_view path)` |
| `std::shared_ptr<`[`VirtualDirectoryResolver`](documentation/generated/Core/VirtualDirectoryResolver.md)`> const&` | `GetResolver()` |
| `bool` | `IsUprootAllowed()` |
| [`VirtualDirectory::DirectoryEntry`](documentation/generated/Core/VirtualDirectory.DirectoryEntry.md)`&` | `StoreDirectory(std::string_view path, std::shared_ptr<`[`VirtualDirectoryResolver`](documentation/generated/Core/VirtualDirectoryResolver.md)`> resolver)` |
| [`VirtualDirectory::DirectoryEntry`](documentation/generated/Core/VirtualDirectory.DirectoryEntry.md)`&` | `StoreDirectory(std::string_view path, Nz::VirtualDirectoryPtr directory)` |
| [`VirtualDirectory::FileEntry`](documentation/generated/Core/VirtualDirectory.FileEntry.md)`&` | `StoreFile(std::string_view path, std::shared_ptr<`[`Stream`](documentation/generated/Core/Stream.md)`> stream)` |
| [`VirtualDirectory::FileEntry`](documentation/generated/Core/VirtualDirectory.FileEntry.md)`&` | `StoreFile(std::string_view path, `[`ByteArray`](documentation/generated/Core/ByteArray.md)` content)` |
| [`VirtualDirectory::FileEntry`](documentation/generated/Core/VirtualDirectory.FileEntry.md)`&` | `StoreFile(std::string_view path, void const* data, std::size_t size)` |
| VirtualDirectory`&` | `operator=(`VirtualDirectory` const&)` |
| VirtualDirectory`&` | `operator=(`VirtualDirectory`&&)` |
