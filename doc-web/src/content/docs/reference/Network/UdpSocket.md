---
title: UdpSocket
description: Nothing
---

# Nz::UdpSocket

Class description

## Constructors

- `UdpSocket()`
- `UdpSocket(Nz::NetProtocol protocol)`
- `UdpSocket(`UdpSocket`&& udpSocket)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::SocketState` | `Bind(Nz::UInt16 port)` |
| `Nz::SocketState` | `Bind(`[`IpAddress`](documentation/generated/Network/IpAddress.md)` const& address)` |
| `bool` | `Create(Nz::NetProtocol protocol)` |
| `void` | `EnableBroadcasting(bool broadcasting)` |
| `Nz::IpAddress` | `GetBoundAddress()` |
| `Nz::UInt16` | `GetBoundPort()` |
| `bool` | `IsBroadcastingEnabled()` |
| `std::size_t` | `QueryMaxDatagramSize()` |
| `bool` | `Receive(void* buffer, std::size_t size, `[`IpAddress`](documentation/generated/Network/IpAddress.md)`* from, std::size_t* received)` |
| `bool` | `ReceiveMultiple(`[`NetBuffer`](documentation/generated/Network/NetBuffer.md)`* buffers, std::size_t bufferCount, `[`IpAddress`](documentation/generated/Network/IpAddress.md)`* from, std::size_t* received)` |
| `bool` | `ReceivePacket(`[`NetPacket`](documentation/generated/Network/NetPacket.md)`* packet, `[`IpAddress`](documentation/generated/Network/IpAddress.md)`* from)` |
| `bool` | `Send(`[`IpAddress`](documentation/generated/Network/IpAddress.md)` const& to, void const* buffer, std::size_t size, std::size_t* sent)` |
| `bool` | `SendMultiple(`[`IpAddress`](documentation/generated/Network/IpAddress.md)` const& to, `[`NetBuffer`](documentation/generated/Network/NetBuffer.md)` const* buffers, std::size_t bufferCount, std::size_t* sent)` |
| `bool` | `SendPacket(`[`IpAddress`](documentation/generated/Network/IpAddress.md)` const& to, `[`NetPacket`](documentation/generated/Network/NetPacket.md)` const& packet)` |
| UdpSocket`&` | `operator=(`UdpSocket` const& udpSocket)` |
| UdpSocket`&` | `operator=(`UdpSocket`&& udpSocket)` |
