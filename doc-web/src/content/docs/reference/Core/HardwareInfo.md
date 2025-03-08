---
title: HardwareInfo
description: Nothing
---

# Nz::HardwareInfo

Class description

## Constructors

- `HardwareInfo()`
- `HardwareInfo(`HardwareInfo` const&)`
- `HardwareInfo(`HardwareInfo`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `char const*` | `GetCpuBrandString()` |
| `unsigned int` | `GetCpuThreadCount()` |
| `Nz::ProcessorVendor` | `GetCpuVendor()` |
| `std::string_view` | `GetCpuVendorName()` |
| `Nz::UInt64` | `GetSystemTotalMemory()` |
| `bool` | `HasCapability(Nz::ProcessorCap capability)` |
| HardwareInfo`&` | `operator=(`HardwareInfo` const&)` |
| HardwareInfo`&` | `operator=(`HardwareInfo`&&)` |
