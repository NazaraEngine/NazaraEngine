---
title: TcpClient
description: Nothing
---

# Nz::TcpClient

Class description

## Constructors

- `TcpClient()`
- `TcpClient(`TcpClient`&& tcpClient)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::SocketState` | `Connect(`[`IpAddress`](documentation/generated/Network/IpAddress.md)` const& remoteAddress)` |
| `Nz::SocketState` | `Connect(std::string const& hostName, Nz::NetProtocol protocol, std::string const& service, Nz::ResolveError* error)` |
| `void` | `Disconnect()` |
| `void` | `EnableLowDelay(bool lowDelay)` |
| `void` | `EnableKeepAlive(bool keepAlive, Nz::UInt64 msTime, Nz::UInt64 msInterval)` |
| `Nz::UInt64` | `GetKeepAliveInterval()` |
| `Nz::UInt64` | `GetKeepAliveTime()` |
| `Nz::IpAddress` | `GetRemoteAddress()` |
| `Nz::UInt64` | `GetSize()` |
| `bool` | `IsLowDelayEnabled()` |
| `bool` | `IsKeepAliveEnabled()` |
| `Nz::SocketState` | `PollForConnected(Nz::UInt64 waitDuration)` |
| `bool` | `Receive(void* buffer, std::size_t size, std::size_t* received)` |
| `bool` | `ReceivePacket(`[`NetPacket`](documentation/generated/Network/NetPacket.md)`* packet)` |
| `bool` | `Send(void const* buffer, std::size_t size, std::size_t* sent)` |
| `bool` | `SendMultiple(`[`NetBuffer`](documentation/generated/Network/NetBuffer.md)` const* buffers, std::size_t bufferCount, std::size_t* sent)` |
| `bool` | `SendPacket(`[`NetPacket`](documentation/generated/Network/NetPacket.md)` const& packet)` |
| `Nz::SocketState` | `WaitForConnected(Nz::UInt64 msTimeout)` |
| TcpClient`&` | `operator=(`TcpClient`&& tcpClient)` |
