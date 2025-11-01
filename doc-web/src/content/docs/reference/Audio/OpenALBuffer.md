---
title: OpenALBuffer
description: Nothing
---

---
title: OpenALBuffer
description: Nothing
---

# Nz::OpenALBuffer

Class description

## Constructors

- `OpenALBuffer(std::shared_ptr<`[`AudioDevice`](documentation/generated/Audio/AudioDevice.md)`> device, `[`OpenALLibrary`](documentation/generated/Audio/OpenALLibrary.md)`& library, ALuint bufferId)`
- `OpenALBuffer(`OpenALBuffer` const&)`
- `OpenALBuffer(`OpenALBuffer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `ALuint` | `GetBufferId()` |
| `Nz::UInt64` | `GetSampleCount()` |
| `Nz::UInt64` | `GetSize()` |
| `Nz::UInt32` | `GetSampleRate()` |
| `bool` | `IsCompatibleWith(`[`AudioDevice`](documentation/generated/Audio/AudioDevice.md)` const& device)` |
| `bool` | `Reset(Nz::AudioFormat format, Nz::UInt64 sampleCount, Nz::UInt32 sampleRate, void const* samples)` |
| OpenALBuffer`&` | `operator=(`OpenALBuffer` const&)` |
| OpenALBuffer`&` | `operator=(`OpenALBuffer`&&)` |
