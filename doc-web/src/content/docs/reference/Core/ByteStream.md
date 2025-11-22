---
title: ByteStream
description: Nothing
---

# Nz::ByteStream

Class description

## Constructors

- `ByteStream(`[`Stream`](documentation/generated/Core/Stream.md)`* stream)`
- `ByteStream(`[`ByteArray`](documentation/generated/Core/ByteArray.md)`* byteArray, Nz::OpenModeFlags openMode)`
- `ByteStream(void* ptr, Nz::UInt64 size)`
- `ByteStream(void const* ptr, Nz::UInt64 size)`
- `ByteStream(`ByteStream` const&)`
- `ByteStream(`ByteStream`&& stream)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `ClearStream()` |
| `Nz::Endianness` | `GetDataEndianness()` |
| `Nz::UInt64` | `GetSize()` |
| [`Stream`](documentation/generated/Core/Stream.md)`*` | `GetStream()` |
| `bool` | `FlushBits()` |
| `std::size_t` | `Read(void* ptr, std::size_t size)` |
| `void` | `SetDataEndianness(Nz::Endianness endiannes)` |
| `void` | `SetStream(`[`Stream`](documentation/generated/Core/Stream.md)`* stream)` |
| `void` | `SetStream(`[`ByteArray`](documentation/generated/Core/ByteArray.md)`* byteArray, Nz::OpenModeFlags openMode)` |
| `void` | `SetStream(void* ptr, Nz::UInt64 size)` |
| `void` | `SetStream(void const* ptr, Nz::UInt64 size)` |
| `std::size_t` | `Write(void const* data, std::size_t size)` |
| ByteStream`&` | `operator=(`ByteStream` const&)` |
| ByteStream`&` | `operator=(`ByteStream`&&)` |
