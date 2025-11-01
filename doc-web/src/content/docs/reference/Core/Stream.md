---
title: Stream
description: Nothing
---

# Nz::Stream

Class description

## Constructors

- `Stream(`Stream` const&)`
- `Stream(`Stream`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `EndOfStream()` |
| `void` | `EnableBuffering(bool buffering, std::size_t bufferSize)` |
| `void` | `EnableTextMode(bool textMode)` |
| `void` | `Flush()` |
| `Nz::UInt64` | `GetCursorPos()` |
| `std::filesystem::path` | `GetDirectory()` |
| `void const*` | `GetMappedPointer()` |
| `void*` | `GetMappedPointerMutable()` |
| `Nz::OpenModeFlags` | `GetOpenMode()` |
| `std::filesystem::path` | `GetPath()` |
| `Nz::UInt64` | `GetSize()` |
| `Nz::StreamOptionFlags` | `GetStreamOptions()` |
| `std::size_t` | `Read(void* buffer, std::size_t size)` |
| `std::string` | `ReadLine(unsigned int lineSize)` |
| `bool` | `IsBufferingEnabled()` |
| `bool` | `IsMemoryMapped()` |
| `bool` | `IsReadable()` |
| `bool` | `IsSequential()` |
| `bool` | `IsTextModeEnabled()` |
| `bool` | `IsWritable()` |
| `bool` | `SetCursorPos(Nz::UInt64 offset)` |
| `bool` | `Write(`[`ByteArray`](documentation/generated/Core/ByteArray.md)` const& byteArray)` |
| `bool` | `Write(std::string_view string)` |
| `std::size_t` | `Write(void const* buffer, std::size_t size)` |
| Stream`&` | `operator=(`Stream` const&)` |
| Stream`&` | `operator=(`Stream`&&)` |
