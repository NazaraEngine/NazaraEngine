---
title: TcpServer
description: Nothing
---

# Nz::TcpServer

Class description

## Constructors

- `TcpServer()`
- `TcpServer(`TcpServer`&& tcpServer)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `AcceptClient(`[`TcpClient`](documentation/generated/Network/TcpClient.md)`* newClient)` |
| `Nz::IpAddress` | `GetBoundAddress()` |
| `Nz::UInt16` | `GetBoundPort()` |
| `Nz::SocketState` | `Listen(Nz::NetProtocol protocol, Nz::UInt16 port, unsigned int queueSize)` |
| `Nz::SocketState` | `Listen(`[`IpAddress`](documentation/generated/Network/IpAddress.md)` const& address, unsigned int queueSize)` |
