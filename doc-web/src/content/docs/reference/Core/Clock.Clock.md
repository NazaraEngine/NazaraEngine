---
title: Clock.Clock
description: Nothing
---

# Nz::Clock::Clock

Class description

## Constructors

- `Clock(`[`Time`](documentation/generated/Core/Time.md)` startingValue, bool paused)`
- `Clock(const Clock<HighPrecision>& clock)`
- `Clock(Clock<HighPrecision>&& clock)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::Time` | `GetElapsedTime()` |
| `bool` | `IsPaused()` |
| `void` | `Pause()` |
| `Nz::Time` | `Restart(`[`Time`](documentation/generated/Core/Time.md)` startingPoint, bool paused)` |
| `std::optional<`[`Time`](documentation/generated/Core/Time.md)`>` | `RestartIfOver(`[`Time`](documentation/generated/Core/Time.md)` time)` |
| `void` | `Unpause()` |
| `Clock<HighPrecision>&` | `operator=(const Clock<HighPrecision>& clock)` |
| `Clock<HighPrecision>&` | `operator=(Clock<HighPrecision>&& clock)` |
