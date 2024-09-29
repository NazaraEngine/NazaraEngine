---
title: OpenALDevice
description: Nothing
---

---
title: OpenALDevice
description: Nothing
---

# Nz::OpenALDevice

Class description

## Constructors

- `OpenALDevice(`[`OpenALLibrary`](documentation/generated/Audio/OpenALLibrary.md)`& library, ALCdevice* device)`
- `OpenALDevice(`OpenALDevice` const&)`
- `OpenALDevice(`OpenALDevice`&&)`

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
| `bool` | `IsExtensionSupported(Nz::OpenALExtension extension)` |
| `bool` | `IsFormatSupported(Nz::AudioFormat format)` |
| `void` | `MakeContextCurrent()` |
| `void` | `SetDopplerFactor(float dopplerFactor)` |
| `void` | `SetGlobalVolume(float volume)` |
| `void` | `SetListenerDirection(Nz::Vector3f const& direction, Nz::Vector3f const& up)` |
| `void` | `SetListenerPosition(Nz::Vector3f const& position)` |
| `void` | `SetListenerVelocity(Nz::Vector3f const& velocity)` |
| `void` | `SetSpeedOfSound(float speed)` |
| `ALenum` | `TranslateAudioFormat(Nz::AudioFormat format)` |
| OpenALDevice`&` | `operator=(`OpenALDevice` const&)` |
| OpenALDevice`&` | `operator=(`OpenALDevice`&&)` |
