---
title: AbstractSocket
description: Nothing
---

# Nz::AbstractSocket

Class description

## Constructors

- `AbstractSocket(`AbstractSocket` const&)`
- `AbstractSocket(`AbstractSocket`&& abstractSocket)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Close()` |
| `void` | `EnableBlocking(bool blocking)` |
| `Nz::SocketError` | `GetLastError()` |
| `Nz::SocketHandle` | `GetNativeHandle()` |
| `Nz::SocketState` | `GetState()` |
| `Nz::SocketType` | `GetType()` |
| `bool` | `IsBlockingEnabled()` |
| `std::size_t` | `QueryAvailableBytes()` |
| `std::size_t` | `QueryReceiveBufferSize()` |
| `std::size_t` | `QuerySendBufferSize()` |
| `void` | `SetReceiveBufferSize(std::size_t size)` |
| `void` | `SetSendBufferSize(std::size_t size)` |
| AbstractSocket`&` | `operator=(`AbstractSocket` const&)` |
| AbstractSocket`&` | `operator=(`AbstractSocket`&& abstractSocket)` |
