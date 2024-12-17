---
title: Music
description: Nothing
---

---
title: Music
description: Nothing
---

# Nz::Music

Class description

## Constructors

- `Music()`
- `Music(`[`AudioDevice`](documentation/generated/Audio/AudioDevice.md)`& device)`
- `Music(`Music` const&)`
- `Music(`Music`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Create(std::shared_ptr<`[`SoundStream`](documentation/generated/Audio/SoundStream.md)`> soundStream)` |
| `void` | `Destroy()` |
| `void` | `EnableLooping(bool loop)` |
| `Nz::Time` | `GetDuration()` |
| `Nz::AudioFormat` | `GetFormat()` |
| `Nz::Time` | `GetPlayingOffset()` |
| `Nz::UInt64` | `GetSampleCount()` |
| `Nz::UInt64` | `GetSampleOffset()` |
| `Nz::UInt32` | `GetSampleRate()` |
| `Nz::SoundStatus` | `GetStatus()` |
| `bool` | `IsLooping()` |
| `bool` | `OpenFromFile(std::filesystem::path const& filePath, `[`SoundStreamParams`](documentation/generated/Audio/SoundStreamParams.md)` const& params)` |
| `bool` | `OpenFromMemory(void const* data, std::size_t size, `[`SoundStreamParams`](documentation/generated/Audio/SoundStreamParams.md)` const& params)` |
| `bool` | `OpenFromStream(`[`Stream`](documentation/generated/Core/Stream.md)`& stream, `[`SoundStreamParams`](documentation/generated/Audio/SoundStreamParams.md)` const& params)` |
| `void` | `Pause()` |
| `void` | `Play()` |
| `void` | `SeekToSampleOffset(Nz::UInt64 offset)` |
| `void` | `Stop()` |
| Music`&` | `operator=(`Music` const&)` |
| Music`&` | `operator=(`Music`&&)` |
