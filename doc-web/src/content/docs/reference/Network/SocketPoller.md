---
title: SocketPoller
description: Nothing
---

# Nz::SocketPoller

Class description

## Constructors

- `SocketPoller()`
- `SocketPoller(`SocketPoller` const&)`
- `SocketPoller(`SocketPoller`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Clear()` |
| `bool` | `IsReadyToRead(`[`AbstractSocket`](documentation/generated/Network/AbstractSocket.md)` const& socket)` |
| `bool` | `IsReadyToWrite(`[`AbstractSocket`](documentation/generated/Network/AbstractSocket.md)` const& socket)` |
| `bool` | `IsRegistered(`[`AbstractSocket`](documentation/generated/Network/AbstractSocket.md)` const& socket)` |
| `bool` | `RegisterSocket(`[`AbstractSocket`](documentation/generated/Network/AbstractSocket.md)`& socket, Nz::SocketPollEventFlags eventFlags)` |
| `void` | `UnregisterSocket(`[`AbstractSocket`](documentation/generated/Network/AbstractSocket.md)`& socket)` |
| `unsigned int` | `Wait(int msTimeout, Nz::SocketError* error)` |
| SocketPoller`&` | `operator=(`SocketPoller` const&)` |
| SocketPoller`&` | `operator=(`SocketPoller`&&)` |
