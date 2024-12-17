---
title: ApplicationBase
description: Nothing
---

# Nz::ApplicationBase

Class description

## Constructors

- `ApplicationBase()`
- `ApplicationBase(int argc, char** argv)`
- `ApplicationBase(int argc, Pointer<const char> const* argv)`
- `ApplicationBase(`ApplicationBase` const&)`
- `ApplicationBase(`ApplicationBase`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `AddUpdater(std::unique_ptr<`[`ApplicationUpdater`](documentation/generated/Core/ApplicationUpdater.md)`>&& functor)` |
| `void` | `ClearComponents()` |
| [`CommandLineParameters`](documentation/generated/Core/CommandLineParameters.md)` const&` | `GetCommandLineParameters()` |
| `void` | `Quit()` |
| `int` | `Run()` |
| `bool` | `Update(`[`Time`](documentation/generated/Core/Time.md)` elapsedTime)` |
| ApplicationBase`&` | `operator=(`ApplicationBase` const&)` |
| ApplicationBase`&` | `operator=(`ApplicationBase`&&)` |
