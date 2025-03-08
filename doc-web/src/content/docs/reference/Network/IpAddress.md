---
title: IpAddress
description: Nothing
---

# Nz::IpAddress

Class description

## Constructors

- `IpAddress()`
- `IpAddress(Nz::IpAddress::IPv4 const& ip, Nz::UInt16 port)`
- `IpAddress(Nz::IpAddress::IPv6 const& ip, Nz::UInt16 port)`
- `IpAddress(Nz::UInt8 const& a, Nz::UInt8 const& b, Nz::UInt8 const& c, Nz::UInt8 const& d, Nz::UInt16 port)`
- `IpAddress(Nz::UInt16 const& a, Nz::UInt16 const& b, Nz::UInt16 const& c, Nz::UInt16 const& d, Nz::UInt16 const& e, Nz::UInt16 const& f, Nz::UInt16 const& g, Nz::UInt16 const& h, Nz::UInt16 port)`
- `IpAddress(char const* address)`
- `IpAddress(std::string const& address)`
- `IpAddress(`IpAddress` const&)`
- `IpAddress(`IpAddress`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `BuildFromAddress(char const* address)` |
| `bool` | `BuildFromAddress(std::string const& address)` |
| `Nz::UInt16` | `GetPort()` |
| `Nz::NetProtocol` | `GetProtocol()` |
| `bool` | `IsLoopback()` |
| `bool` | `IsValid()` |
| `void` | `SetPort(Nz::UInt16 port)` |
| `IPv4` | `ToIPv4()` |
| `IPv6` | `ToIPv6()` |
| `std::string` | `ToString()` |
| `Nz::UInt32` | `ToUInt32()` |
| IpAddress`&` | `operator=(`IpAddress` const&)` |
| IpAddress`&` | `operator=(`IpAddress`&&)` |
