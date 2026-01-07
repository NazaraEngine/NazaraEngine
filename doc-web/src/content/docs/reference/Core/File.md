---
title: File
description: Nothing
---

# Nz::File

Class description

## Constructors

- `File()`
- `File(std::filesystem::path const& filePath)`
- `File(std::filesystem::path const& filePath, Nz::OpenModeFlags openMode)`
- `File(`File` const&)`
- `File(`File`&& file)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Copy(std::filesystem::path const& newFilePath)` |
| `void` | `Close()` |
| `bool` | `Delete()` |
| `bool` | `Exists()` |
| `std::filesystem::path` | `GetDirectory()` |
| `std::filesystem::path` | `GetFileName()` |
| `std::filesystem::path` | `GetPath()` |
| `Nz::UInt64` | `GetSize()` |
| `bool` | `IsOpen()` |
| `bool` | `Open(Nz::OpenModeFlags openMode)` |
| `bool` | `Open(std::filesystem::path const& filePath, Nz::OpenModeFlags openMode)` |
| `bool` | `SetFile(std::filesystem::path const& filePath)` |
| `bool` | `SetSize(Nz::UInt64 size)` |
| File`&` | `operator=(`File` const&)` |
| File`&` | `operator=(`File`&& file)` |
