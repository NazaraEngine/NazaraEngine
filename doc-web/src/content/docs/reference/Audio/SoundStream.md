---
title: SoundStream
description: Nothing
---

---
title: SoundStream
description: Nothing
---

# Nz::SoundStream

Class description

## Constructors

- `SoundStream()`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::Time` | `GetDuration()` |
| `Nz::AudioFormat` | `GetFormat()` |
| `std::mutex&` | `GetMutex()` |
| `Nz::UInt64` | `GetSampleCount()` |
| `Nz::UInt32` | `GetSampleRate()` |
| `Nz::UInt64` | `Read(void* buffer, Nz::UInt64 sampleCount)` |
| `void` | `Seek(Nz::UInt64 offset)` |
| `Nz::UInt64` | `Tell()` |
