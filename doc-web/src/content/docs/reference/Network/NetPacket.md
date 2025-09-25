---
title: NetPacket
description: Nothing
---

# Nz::NetPacket

Class description

## Constructors

- `NetPacket()`
- `NetPacket(Nz::UInt16 netCode, std::size_t minCapacity)`
- `NetPacket(Nz::UInt16 netCode, void const* ptr, std::size_t size)`
- `NetPacket(`NetPacket` const&)`
- `NetPacket(`NetPacket`&& packet)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::UInt8 const*` | `GetConstData()` |
| `Nz::UInt8*` | `GetData()` |
| `size_t` | `GetDataSize()` |
| `Nz::UInt16` | `GetNetCode()` |
| `void` | `OnReceive(Nz::UInt16 netCode, void const* data, std::size_t size)` |
| `void const*` | `OnSend(std::size_t* newSize)` |
| `void` | `Reset()` |
| `void` | `Reset(Nz::UInt16 netCode, std::size_t minCapacity)` |
| `void` | `Reset(Nz::UInt16 netCode, void const* ptr, std::size_t size)` |
| `void` | `Resize(std::size_t newSize)` |
| `void` | `SetNetCode(Nz::UInt16 netCode)` |
| NetPacket`&` | `operator=(`NetPacket` const&)` |
| NetPacket`&` | `operator=(`NetPacket`&& packet)` |
