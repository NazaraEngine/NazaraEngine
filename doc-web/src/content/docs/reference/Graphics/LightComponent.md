---
title: LightComponent
description: Nothing
---

# Nz::LightComponent

Class description

## Constructors

- `LightComponent(bool initialyVisible)`
- `LightComponent(`LightComponent` const&)`
- `LightComponent(`LightComponent`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Clear()` |
| [`LightComponent::LightEntry`](documentation/generated/Graphics/LightComponent.LightEntry.md)` const&` | `GetLightEntry(std::size_t lightIndex)` |
| `std::array<LightEntry, MaxLightCount> const&` | `GetLights()` |
| `void` | `Hide()` |
| `bool` | `IsVisible()` |
| `void` | `RemoveLight(std::size_t lightIndex)` |
| `void` | `RemoveLight(`[`Light`](documentation/generated/Graphics/Light.md)` const& renderable)` |
| `void` | `Show(bool show)` |
| LightComponent`&` | `operator=(`LightComponent` const&)` |
| LightComponent`&` | `operator=(`LightComponent`&&)` |
