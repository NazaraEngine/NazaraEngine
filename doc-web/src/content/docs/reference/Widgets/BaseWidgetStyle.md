---
title: BaseWidgetStyle
description: Nothing
---

# Nz::BaseWidgetStyle

Class description

## Constructors

- `BaseWidgetStyle(`[`BaseWidget`](documentation/generated/Widgets/BaseWidget.md)`* widget, int renderLayerCount)`
- `BaseWidgetStyle(`BaseWidgetStyle` const&)`
- `BaseWidgetStyle(`BaseWidgetStyle`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `entt::entity` | `CreateEntity()` |
| `entt::entity` | `CreateGraphicsEntity()` |
| `void` | `DestroyEntity(entt::entity entity)` |
| `entt::registry&` | `GetRegistry()` |
| `entt::registry const&` | `GetRegistry()` |
| `Nz::UInt32` | `GetRenderMask()` |
| `int` | `GetRenderLayerCount()` |
| `void` | `UpdateRenderLayer(int baseRenderLayer)` |
| BaseWidgetStyle`&` | `operator=(`BaseWidgetStyle` const&)` |
| BaseWidgetStyle`&` | `operator=(`BaseWidgetStyle`&&)` |
