---
title: DummyAudioSource
description: Nothing
---

---
title: DummyAudioSource
description: Nothing
---

# Nz::DummyAudioSource

Class description

## Constructors

- `DummyAudioSource(std::shared_ptr<`[`AudioDevice`](documentation/generated/Audio/AudioDevice.md)`> device)`
- `DummyAudioSource(`DummyAudioSource` const&)`
- `DummyAudioSource(`DummyAudioSource`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `EnableLooping(bool loop)` |
| `void` | `EnableSpatialization(bool spatialization)` |
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
| DummyAudioSource`&` | `operator=(`DummyAudioSource` const&)` |
| DummyAudioSource`&` | `operator=(`DummyAudioSource`&&)` |
