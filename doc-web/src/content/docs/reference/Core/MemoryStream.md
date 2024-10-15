---
title: MemoryStream
description: Nothing
---

# Nz::MemoryStream

Class description

## Constructors

- `MemoryStream()`
- `MemoryStream(`[`ByteArray`](documentation/generated/Core/ByteArray.md)`* byteArray, Nz::OpenModeFlags openMode)`
- `MemoryStream(`MemoryStream` const&)`
- `MemoryStream(`MemoryStream`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Clear()` |
| [`ByteArray`](documentation/generated/Core/ByteArray.md)`&` | `GetBuffer()` |
| [`ByteArray`](documentation/generated/Core/ByteArray.md)` const&` | `GetBuffer()` |
| `Nz::UInt64` | `GetSize()` |
| `void` | `SetBuffer(`[`ByteArray`](documentation/generated/Core/ByteArray.md)`* byteArray, Nz::OpenModeFlags openMode)` |
| MemoryStream`&` | `operator=(`MemoryStream` const&)` |
| MemoryStream`&` | `operator=(`MemoryStream`&&)` |
