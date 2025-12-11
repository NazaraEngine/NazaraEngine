---
title: Audio
description: Nothing
---

---
title: Audio
description: Nothing
---

# Nz::Audio

Class description

## Constructors

- `Audio(`[`Audio::Config`](documentation/generated/Audio/Audio.Config.md)` config)`
- `Audio(`Audio` const&)`
- `Audio(`Audio`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::shared_ptr<`[`AudioDevice`](documentation/generated/Audio/AudioDevice.md)`> const&` | `GetDefaultDevice()` |
| `Nz::SoundBufferLoader&` | `GetSoundBufferLoader()` |
| `Nz::SoundBufferLoader const&` | `GetSoundBufferLoader()` |
| `Nz::SoundStreamLoader&` | `GetSoundStreamLoader()` |
| `Nz::SoundStreamLoader const&` | `GetSoundStreamLoader()` |
| `std::shared_ptr<`[`AudioDevice`](documentation/generated/Audio/AudioDevice.md)`>` | `OpenOutputDevice(std::string const& deviceName)` |
| `std::vector<std::string>` | `QueryInputDevices()` |
| `std::vector<std::string>` | `QueryOutputDevices()` |
| Audio`&` | `operator=(`Audio` const&)` |
| Audio`&` | `operator=(`Audio`&&)` |
