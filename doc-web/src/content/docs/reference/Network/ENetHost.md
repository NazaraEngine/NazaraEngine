---
title: ENetHost
description: Nothing
---

# Nz::ENetHost

Class description

## Constructors

- `ENetHost()`
- `ENetHost(`ENetHost` const&)`
- `ENetHost(`ENetHost`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::ENetPacketRef` | `AllocatePacket(Nz::ENetPacketFlags flags)` |
| `Nz::ENetPacketRef` | `AllocatePacket(Nz::ENetPacketFlags flags, `[`NetPacket`](documentation/generated/Network/NetPacket.md)`&& data)` |
| `void` | `AllowsIncomingConnections(bool allow)` |
| `void` | `Broadcast(Nz::UInt8 channelId, Nz::ENetPacketFlags flags, `[`NetPacket`](documentation/generated/Network/NetPacket.md)`&& packet)` |
| `bool` | `CheckEvents(`[`ENetEvent`](documentation/generated/Network/ENetEvent.md)`* event)` |
| [`ENetPeer`](documentation/generated/Network/ENetPeer.md)`*` | `Connect(`[`IpAddress`](documentation/generated/Network/IpAddress.md)` const& remoteAddress, std::size_t channelCount, Nz::UInt32 data)` |
| [`ENetPeer`](documentation/generated/Network/ENetPeer.md)`*` | `Connect(std::string const& hostName, Nz::NetProtocol protocol, std::string const& service, Nz::ResolveError* error, std::size_t channelCount, Nz::UInt32 data)` |
| `bool` | `Create(Nz::NetProtocol protocol, Nz::UInt16 port, std::size_t peerCount, std::size_t channelCount)` |
| `bool` | `Create(`[`IpAddress`](documentation/generated/Network/IpAddress.md)` const& listenAddress, std::size_t peerCount, std::size_t channelCount)` |
| `bool` | `Create(`[`IpAddress`](documentation/generated/Network/IpAddress.md)` const& listenAddress, std::size_t peerCount, std::size_t channelCount, Nz::UInt32 incomingBandwidth, Nz::UInt32 outgoingBandwidth)` |
| `void` | `Destroy()` |
| `bool` | `DoesAllowIncomingConnections()` |
| `void` | `Flush()` |
| `Nz::IpAddress` | `GetBoundAddress()` |
| `Nz::UInt32` | `GetServiceTime()` |
| `Nz::UInt32` | `GetTotalReceivedPackets()` |
| `Nz::UInt64` | `GetTotalReceivedData()` |
| `Nz::UInt64` | `GetTotalSentData()` |
| `Nz::UInt32` | `GetTotalSentPackets()` |
| `int` | `Service(`[`ENetEvent`](documentation/generated/Network/ENetEvent.md)`* event, Nz::UInt32 timeout)` |
| `void` | `SetCompressor(std::unique_ptr<`[`ENetCompressor`](documentation/generated/Network/ENetCompressor.md)`>&& compressor)` |
| `void` | `SimulateNetwork(double packetLossProbability, Nz::UInt16 minDelay, Nz::UInt16 maxDelay)` |
| ENetHost`&` | `operator=(`ENetHost` const&)` |
| ENetHost`&` | `operator=(`ENetHost`&&)` |
