---
title: AudioSource
description: Nothing
---

---
title: AudioSource
description: Nothing
---

# Nz::AudioSource

Class description

## Constructors

- `AudioSource(std::shared_ptr<`[`AudioDevice`](documentation/generated/Audio/AudioDevice.md)`> device)`
- `AudioSource(`AudioSource` const&)`
- `AudioSource(`AudioSource`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `EnableLooping(bool loop)` |
| `void` | `EnableSpatialization(bool spatialization)` |
| `std::shared_ptr<`[`AudioDevice`](documentation/generated/Audio/AudioDevice.md)`> const&` | `GetAudioDevice()` |
| `float` | `GetAttenuation()` |
| `float` | `GetMinDistance()` |
| `float` | `GetPitch()` |
| `Nz::Time` | `GetPlayingOffset()` |
| `Nz::Vector3f` | `GetPosition()` |
| `Nz::UInt32` | `GetSampleOffset()` |
| `Nz::AudioSource::OffsetWithLatency` | `GetSampleOffsetAndLatency()` |
| `Nz::Vector3f` | `GetVelocity()` |
| `Nz::SoundStatus` | `GetStatus()` |
| `float` | `GetVolume()` |
| `bool` | `IsLooping()` |
| `bool` | `IsSpatializationEnabled()` |
| `void` | `QueueBuffer(std::shared_ptr<`[`AudioBuffer`](documentation/generated/Audio/AudioBuffer.md)`> audioBuffer)` |
| `void` | `Pause()` |
| `void` | `Play()` |
| `void` | `SetAttenuation(float attenuation)` |
| `void` | `SetBuffer(std::shared_ptr<`[`AudioBuffer`](documentation/generated/Audio/AudioBuffer.md)`> audioBuffer)` |
| `void` | `SetMinDistance(float minDistance)` |
| `void` | `SetPitch(float pitch)` |
| `void` | `SetPlayingOffset(`[`Time`](documentation/generated/Core/Time.md)` offset)` |
| `void` | `SetPosition(Nz::Vector3f const& position)` |
| `void` | `SetSampleOffset(Nz::UInt32 offset)` |
| `void` | `SetVelocity(Nz::Vector3f const& velocity)` |
| `void` | `SetVolume(float volume)` |
| `void` | `Stop()` |
| `std::shared_ptr<`[`AudioBuffer`](documentation/generated/Audio/AudioBuffer.md)`>` | `TryUnqueueProcessedBuffer()` |
| `void` | `UnqueueAllBuffers()` |
| AudioSource`&` | `operator=(`AudioSource` const&)` |
| AudioSource`&` | `operator=(`AudioSource`&&)` |
