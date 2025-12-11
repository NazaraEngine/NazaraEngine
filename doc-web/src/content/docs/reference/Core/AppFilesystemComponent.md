---
title: AppFilesystemComponent
description: Nothing
---

# Nz::AppFilesystemComponent

Class description

## Constructors

- `AppFilesystemComponent(`[`ApplicationBase`](documentation/generated/Core/ApplicationBase.md)`& app)`
- `AppFilesystemComponent(`AppFilesystemComponent` const&)`
- `AppFilesystemComponent(`AppFilesystemComponent`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::VirtualDirectoryPtr` | `GetDirectory(std::string_view assetPath)` |
| `Nz::VirtualDirectoryPtr const&` | `Mount(std::string_view name, std::filesystem::path filepath)` |
| `Nz::VirtualDirectoryPtr const&` | `Mount(std::string_view name, Nz::VirtualDirectoryPtr directory)` |
| `void` | `MountDefaultDirectories()` |
| AppFilesystemComponent`&` | `operator=(`AppFilesystemComponent` const&)` |
| AppFilesystemComponent`&` | `operator=(`AppFilesystemComponent`&&)` |
