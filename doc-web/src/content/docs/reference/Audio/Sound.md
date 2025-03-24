---
title: Sound
description: Nothing
---

---
title: Sound
description: Nothing
---

# Nz::Sound

Class description

## Constructors

- `Sound()`
- `Sound(`[`AudioDevice`](documentation/generated/Audio/AudioDevice.md)`& audioDevice, std::shared_ptr<`[`SoundBuffer`](documentation/generated/Audio/SoundBuffer.md)`> soundBuffer)`
- `Sound(`Sound` const&)`
- `Sound(`Sound`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `EnableLooping(bool loop)` |
| `std::shared_ptr<`[`SoundBuffer`](documentation/generated/Audio/SoundBuffer.md)`> const&` | `GetBuffer()` |
| `Nz::Time` | `GetDuration()` |
| `Nz::Time` | `GetPlayingOffset()` |
| `Nz::UInt64` | `GetSampleOffset()` |
| `Nz::UInt32` | `GetSampleRate()` |
| `Nz::SoundStatus` | `GetStatus()` |
| `bool` | `IsLooping()` |
| `bool` | `IsPlayable()` |
| `bool` | `LoadFromFile(std::filesystem::path const& filePath, `[`SoundBufferParams`](documentation/generated/Audio/SoundBufferParams.md)` const& params)` |
| `bool` | `LoadFromMemory(void const* data, std::size_t size, `[`SoundBufferParams`](documentation/generated/Audio/SoundBufferParams.md)` const& params)` |
| `bool` | `LoadFromStream(`[`Stream`](documentation/generated/Core/Stream.md)`& stream, `[`SoundBufferParams`](documentation/generated/Audio/SoundBufferParams.md)` const& params)` |
| `void` | `Pause()` |
| `void` | `Play()` |
| `void` | `SetBuffer(std::shared_ptr<`[`SoundBuffer`](documentation/generated/Audio/SoundBuffer.md)`> soundBuffer)` |
| `void` | `SeekToSampleOffset(Nz::UInt64 offset)` |
| `void` | `Stop()` |
| Sound`&` | `operator=(`Sound` const&)` |
| Sound`&` | `operator=(`Sound`&&)` |
