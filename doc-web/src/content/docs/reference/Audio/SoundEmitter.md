---
title: SoundEmitter
description: Nothing
---

---
title: SoundEmitter
description: Nothing
---

# Nz::SoundEmitter

Class description

## Constructors

- `SoundEmitter(`[`AudioDevice`](documentation/generated/Audio/AudioDevice.md)`& audioDevice)`
- `SoundEmitter(`SoundEmitter` const&)`
- `SoundEmitter(`SoundEmitter`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `EnableLooping(bool loop)` |
| `void` | `EnableSpatialization(bool spatialization)` |
| `float` | `GetAttenuation()` |
| `Nz::Time` | `GetDuration()` |
| `float` | `GetMinDistance()` |
| `float` | `GetPitch()` |
| `Nz::Time` | `GetPlayingOffset()` |
| `Nz::Vector3f` | `GetPosition()` |
| `Nz::UInt64` | `GetSampleOffset()` |
| `Nz::UInt32` | `GetSampleRate()` |
| `Nz::Vector3f` | `GetVelocity()` |
| `Nz::SoundStatus` | `GetStatus()` |
| `float` | `GetVolume()` |
| `bool` | `IsLooping()` |
| `bool` | `IsPlaying()` |
| `bool` | `IsSpatializationEnabled()` |
| `void` | `Pause()` |
| `void` | `Play()` |
| `void` | `SeekToPlayingOffset(`[`Time`](documentation/generated/Core/Time.md)` offset)` |
| `void` | `SeekToSampleOffset(Nz::UInt64 offset)` |
| `void` | `SetAttenuation(float attenuation)` |
| `void` | `SetMinDistance(float minDistance)` |
| `void` | `SetPitch(float pitch)` |
| `void` | `SetPosition(Nz::Vector3f const& position)` |
| `void` | `SetVelocity(Nz::Vector3f const& velocity)` |
| `void` | `SetVolume(float volume)` |
| `void` | `Stop()` |
| SoundEmitter`&` | `operator=(`SoundEmitter` const&)` |
| SoundEmitter`&` | `operator=(`SoundEmitter`&&)` |
