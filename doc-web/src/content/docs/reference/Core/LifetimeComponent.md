---
title: LifetimeComponent
description: Nothing
---

# Nz::LifetimeComponent

Class description

## Constructors

- `LifetimeComponent(`[`Time`](documentation/generated/Core/Time.md)` lifetime)`
- `LifetimeComponent(`LifetimeComponent` const&)`
- `LifetimeComponent(`LifetimeComponent`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `DecreaseLifetime(`[`Time`](documentation/generated/Core/Time.md)` elapsedTime)` |
| `Nz::Time` | `GetRemainingLifeTime()` |
| `bool` | `IsAlive()` |
| LifetimeComponent`&` | `operator=(`LifetimeComponent` const&)` |
| LifetimeComponent`&` | `operator=(`LifetimeComponent`&&)` |
