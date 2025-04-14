---
title: SimpleLabelWidgetStyle
description: Nothing
---

# Nz::SimpleLabelWidgetStyle

Class description

## Constructors

- `SimpleLabelWidgetStyle(`[`LabelWidget`](documentation/generated/Widgets/LabelWidget.md)`* labelWidget, std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> material, std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> hoveredMaterial)`
- `SimpleLabelWidgetStyle(`SimpleLabelWidgetStyle` const&)`
- `SimpleLabelWidgetStyle(`SimpleLabelWidgetStyle`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Layout(Nz::Vector2f const& size)` |
| `void` | `OnHoverBegin()` |
| `void` | `OnHoverEnd()` |
| `void` | `UpdateRenderLayer(int baseRenderLayer)` |
| `void` | `UpdateText(`[`AbstractTextDrawer`](documentation/generated/Utility/AbstractTextDrawer.md)` const& drawer)` |
| SimpleLabelWidgetStyle`&` | `operator=(`SimpleLabelWidgetStyle` const&)` |
| SimpleLabelWidgetStyle`&` | `operator=(`SimpleLabelWidgetStyle`&&)` |
