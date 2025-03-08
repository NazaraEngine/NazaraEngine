---
title: ENetPacketRef
description: Nothing
---

# Nz::ENetPacketRef

Class description

## Constructors

- `ENetPacketRef()`
- `ENetPacketRef(MemoryPool<`[`ENetPacket`](documentation/generated/Network/ENetPacket.md)`>* pool, `[`ENetPacket`](documentation/generated/Network/ENetPacket.md)`* packet)`
- `ENetPacketRef(`ENetPacketRef` const& packet)`
- `ENetPacketRef(`ENetPacketRef`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Reset(`[`ENetPacket`](documentation/generated/Network/ENetPacket.md)`* packet)` |
| [`ENetPacket`](documentation/generated/Network/ENetPacket.md)`*` | `operator->()` |
| ENetPacketRef`&` | `operator=(`ENetPacketRef` const& packet)` |
| ENetPacketRef`&` | `operator=(`ENetPacketRef`&&)` |
