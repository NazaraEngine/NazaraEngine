---
title: AudioBuffer
description: Nothing
---

---
title: AudioBuffer
description: Nothing
---

# Nz::AudioBuffer

Class description

## Constructors

- `AudioBuffer(std::shared_ptr<`[`AudioDevice`](documentation/generated/Audio/AudioDevice.md)`> device)`
- `AudioBuffer(`AudioBuffer` const&)`
- `AudioBuffer(`AudioBuffer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::shared_ptr<`[`AudioDevice`](documentation/generated/Audio/AudioDevice.md)`> const&` | `GetAudioDevice()` |
| `Nz::UInt64` | `GetSampleCount()` |
| `Nz::UInt64` | `GetSize()` |
| `Nz::UInt32` | `GetSampleRate()` |
| `bool` | `IsCompatibleWith(`[`AudioDevice`](documentation/generated/Audio/AudioDevice.md)` const& device)` |
| `bool` | `Reset(Nz::AudioFormat format, Nz::UInt64 sampleCount, Nz::UInt32 sampleRate, void const* samples)` |
| AudioBuffer`&` | `operator=(`AudioBuffer` const&)` |
| AudioBuffer`&` | `operator=(`AudioBuffer`&&)` |
