---
title: SoundBuffer
description: Nothing
---

---
title: SoundBuffer
description: Nothing
---

# Nz::SoundBuffer

Class description

## Constructors

- `SoundBuffer()`
- `SoundBuffer(Nz::AudioFormat format, Nz::UInt64 sampleCount, Nz::UInt32 sampleRate, Nz::Int16 const* samples)`
- `SoundBuffer(`SoundBuffer` const&)`
- `SoundBuffer(`SoundBuffer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::shared_ptr<`[`AudioBuffer`](documentation/generated/Audio/AudioBuffer.md)`> const&` | `GetAudioBuffer(`[`AudioDevice`](documentation/generated/Audio/AudioDevice.md)`* device)` |
| `Nz::Time` | `GetDuration()` |
| `Nz::AudioFormat` | `GetFormat()` |
| `Nz::Int16 const*` | `GetSamples()` |
| `Nz::UInt64` | `GetSampleCount()` |
| `Nz::UInt32` | `GetSampleRate()` |
| SoundBuffer`&` | `operator=(`SoundBuffer` const&)` |
| SoundBuffer`&` | `operator=(`SoundBuffer`&&)` |
