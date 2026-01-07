---
title: AudioDevice
description: Nothing
---

---
title: AudioDevice
description: Nothing
---

# Nz::AudioDevice

Class description

## Constructors

- `AudioDevice()`
- `AudioDevice(`AudioDevice` const&)`
- `AudioDevice(`AudioDevice`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::shared_ptr<`[`AudioBuffer`](documentation/generated/Audio/AudioBuffer.md)`>` | `CreateBuffer()` |
| `std::shared_ptr<`[`AudioSource`](documentation/generated/Audio/AudioSource.md)`>` | `CreateSource()` |
| `float` | `GetDopplerFactor()` |
| `float` | `GetGlobalVolume()` |
| `Nz::Vector3f` | `GetListenerDirection(Nz::Vector3f* up)` |
| `Nz::Vector3f` | `GetListenerPosition()` |
| `Nz::Quaternionf` | `GetListenerRotation()` |
| `Nz::Vector3f` | `GetListenerVelocity()` |
| `float` | `GetSpeedOfSound()` |
| `void const*` | `GetSubSystemIdentifier()` |
| `bool` | `IsFormatSupported(Nz::AudioFormat format)` |
| `void` | `SetDopplerFactor(float dopplerFactor)` |
| `void` | `SetGlobalVolume(float volume)` |
| `void` | `SetListenerDirection(Nz::Vector3f const& direction, Nz::Vector3f const& up)` |
| `void` | `SetListenerPosition(Nz::Vector3f const& position)` |
| `void` | `SetListenerRotation(Nz::Quaternionf const& rotation)` |
| `void` | `SetListenerVelocity(Nz::Vector3f const& velocity)` |
| `void` | `SetSpeedOfSound(float speed)` |
| AudioDevice`&` | `operator=(`AudioDevice` const&)` |
| AudioDevice`&` | `operator=(`AudioDevice`&&)` |
