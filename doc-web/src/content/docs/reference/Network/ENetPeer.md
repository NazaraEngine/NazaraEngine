---
title: ENetPeer
description: Nothing
---

# Nz::ENetPeer

Class description

## Constructors

- `ENetPeer(`[`ENetHost`](documentation/generated/Network/ENetHost.md)`* host, Nz::UInt16 peerId)`
- `ENetPeer(`ENetPeer` const&)`
- `ENetPeer(`ENetPeer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Disconnect(Nz::UInt32 data)` |
| `void` | `DisconnectLater(Nz::UInt32 data)` |
| `void` | `DisconnectNow(Nz::UInt32 data)` |
| [`IpAddress`](documentation/generated/Network/IpAddress.md)` const&` | `GetAddress()` |
| `Nz::UInt32` | `GetLastReceiveTime()` |
| `Nz::UInt32` | `GetMtu()` |
| `Nz::UInt32` | `GetPacketThrottleAcceleration()` |
| `Nz::UInt32` | `GetPacketThrottleDeceleration()` |
| `Nz::UInt32` | `GetPacketThrottleInterval()` |
| `Nz::UInt16` | `GetPeerId()` |
| `Nz::UInt32` | `GetRoundTripTime()` |
| `Nz::ENetPeerState` | `GetState()` |
| `Nz::UInt64` | `GetTotalByteReceived()` |
| `Nz::UInt64` | `GetTotalByteSent()` |
| `Nz::UInt32` | `GetTotalPacketReceived()` |
| `Nz::UInt32` | `GetTotalPacketLost()` |
| `Nz::UInt32` | `GetTotalPacketSent()` |
| `bool` | `HasPendingCommands()` |
| `bool` | `IsConnected()` |
| `bool` | `IsSimulationEnabled()` |
| `void` | `Ping()` |
| `bool` | `Receive(`[`ENetPacketRef`](documentation/generated/Network/ENetPacketRef.md)`* packet, Nz::UInt8* channelId)` |
| `void` | `Reset()` |
| `bool` | `Send(Nz::UInt8 channelId, `[`ENetPacketRef`](documentation/generated/Network/ENetPacketRef.md)` packetRef)` |
| `bool` | `Send(Nz::UInt8 channelId, Nz::ENetPacketFlags flags, `[`NetPacket`](documentation/generated/Network/NetPacket.md)`&& packet)` |
| `void` | `SimulateNetwork(double packetLossProbability, Nz::UInt16 minDelay, Nz::UInt16 maxDelay)` |
| `void` | `ThrottleConfigure(Nz::UInt32 interval, Nz::UInt32 acceleration, Nz::UInt32 deceleration)` |
| ENetPeer`&` | `operator=(`ENetPeer` const&)` |
| ENetPeer`&` | `operator=(`ENetPeer`&&)` |
