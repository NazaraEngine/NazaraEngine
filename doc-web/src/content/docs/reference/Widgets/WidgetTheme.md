---
title: WidgetTheme
description: Nothing
---

# Nz::WidgetTheme

Class description

## Constructors

- `WidgetTheme()`
- `WidgetTheme(`WidgetTheme` const&)`
- `WidgetTheme(`WidgetTheme`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::unique_ptr<`[`ButtonWidgetStyle`](documentation/generated/Widgets/ButtonWidgetStyle.md)`>` | `CreateStyle(`[`ButtonWidget`](documentation/generated/Widgets/ButtonWidget.md)`* buttonWidget)` |
| `std::unique_ptr<`[`CheckboxWidgetStyle`](documentation/generated/Widgets/CheckboxWidgetStyle.md)`>` | `CreateStyle(`[`CheckboxWidget`](documentation/generated/Widgets/CheckboxWidget.md)`* checkboxWidget)` |
| `std::unique_ptr<`[`ImageButtonWidgetStyle`](documentation/generated/Widgets/ImageButtonWidgetStyle.md)`>` | `CreateStyle(`[`ImageButtonWidget`](documentation/generated/Widgets/ImageButtonWidget.md)`* imageButtonWidget)` |
| `std::unique_ptr<`[`LabelWidgetStyle`](documentation/generated/Widgets/LabelWidgetStyle.md)`>` | `CreateStyle(`[`LabelWidget`](documentation/generated/Widgets/LabelWidget.md)`* labelWidget)` |
| `std::unique_ptr<`[`ScrollAreaWidgetStyle`](documentation/generated/Widgets/ScrollAreaWidgetStyle.md)`>` | `CreateStyle(`[`ScrollAreaWidget`](documentation/generated/Widgets/ScrollAreaWidget.md)`* scrollareaWidget)` |
| `std::unique_ptr<`[`ScrollbarWidgetStyle`](documentation/generated/Widgets/ScrollbarWidgetStyle.md)`>` | `CreateStyle(`[`ScrollbarWidget`](documentation/generated/Widgets/ScrollbarWidget.md)`* scrollbarWidget)` |
| `std::unique_ptr<`[`ScrollbarButtonWidgetStyle`](documentation/generated/Widgets/ScrollbarButtonWidgetStyle.md)`>` | `CreateStyle(`[`ScrollbarButtonWidget`](documentation/generated/Widgets/ScrollbarButtonWidget.md)`* scrollbarButtonWidget)` |
| [`WidgetTheme::Config`](documentation/generated/Widgets/WidgetTheme.Config.md)` const&` | `GetConfig()` |
| WidgetTheme`&` | `operator=(`WidgetTheme` const&)` |
| WidgetTheme`&` | `operator=(`WidgetTheme`&&)` |
