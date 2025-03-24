---
title: ENetCompressor
description: Nothing
---

# Nz::ENetCompressor

Class description

## Constructors

- `ENetCompressor()`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::size_t` | `Compress(`[`ENetPeer`](documentation/generated/Network/ENetPeer.md)` const* peer, `[`NetBuffer`](documentation/generated/Network/NetBuffer.md)` const* buffers, std::size_t bufferCount, std::size_t totalInputSize, Nz::UInt8* output, std::size_t maxOutputSize)` |
| `std::size_t` | `Decompress(`[`ENetPeer`](documentation/generated/Network/ENetPeer.md)` const* peer, Nz::UInt8 const* input, std::size_t inputSize, Nz::UInt8* output, std::size_t maxOutputSize)` |
