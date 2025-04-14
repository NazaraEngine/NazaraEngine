---
title: OpenALLibrary
description: Nothing
---

---
title: OpenALLibrary
description: Nothing
---

# Nz::OpenALLibrary

Class description

## Constructors

- `OpenALLibrary()`
- `OpenALLibrary(`OpenALLibrary` const&)`
- `OpenALLibrary(`OpenALLibrary`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `IsLoaded()` |
| `bool` | `Load()` |
| `std::vector<std::string>` | `QueryInputDevices()` |
| `std::vector<std::string>` | `QueryOutputDevices()` |
| `std::shared_ptr<`[`OpenALDevice`](documentation/generated/Audio/OpenALDevice.md)`>` | `OpenDevice(char const* name)` |
| `void` | `Unload()` |
| OpenALLibrary`&` | `operator=(`OpenALLibrary` const&)` |
| OpenALLibrary`&` | `operator=(`OpenALLibrary`&&)` |
