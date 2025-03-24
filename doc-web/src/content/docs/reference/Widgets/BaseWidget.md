---
title: BaseWidget
description: Nothing
---

# Nz::BaseWidget

Class description

## Constructors

- `BaseWidget(`BaseWidget`* parent)`
- `BaseWidget(`BaseWidget` const&)`
- `BaseWidget(`BaseWidget`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `AddChild(std::unique_ptr<`BaseWidget`>&& widget)` |
| `void` | `Center()` |
| `void` | `CenterHorizontal()` |
| `void` | `CenterVertical()` |
| `void` | `ClearFocus()` |
| `void` | `ClearRenderingRect()` |
| `void` | `Destroy()` |
| `void` | `EnableBackground(bool enable)` |
| [`Color`](documentation/generated/Core/Color.md)` const&` | `GetBackgroundColor()` |
| [`Canvas`](documentation/generated/Widgets/Canvas.md)`*` | `GetCanvas()` |
| `Nz::SystemCursor` | `GetCursor()` |
| `float` | `GetHeight()` |
| `float` | `GetMaximumHeight()` |
| `Nz::Vector2f` | `GetMaximumSize()` |
| `float` | `GetMaximumWidth()` |
| `float` | `GetMinimumHeight()` |
| `Nz::Vector2f` | `GetMinimumSize()` |
| `float` | `GetMinimumWidth()` |
| `float` | `GetPreferredHeight()` |
| `Nz::Vector2f` | `GetPreferredSize()` |
| `float` | `GetPreferredWidth()` |
| `Nz::Rectf const&` | `GetRenderingRect()` |
| `Nz::Vector2f` | `GetSize()` |
| `std::shared_ptr<`[`WidgetTheme`](documentation/generated/Widgets/WidgetTheme.md)`> const&` | `GetTheme()` |
| `std::size_t` | `GetVisibleWidgetChildCount()` |
| `float` | `GetWidth()` |
| `std::size_t` | `GetWidgetChildCount()` |
| `bool` | `HasFocus()` |
| `void` | `Hide()` |
| `bool` | `IsVisible()` |
| `std::unique_ptr<`BaseWidget`>` | `ReleaseFromParent()` |
| `void` | `Resize(Nz::Vector2f const& size)` |
| `void` | `SetBackgroundColor(`[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `SetCursor(Nz::SystemCursor systemCursor)` |
| `void` | `SetFocus()` |
| `void` | `SetFixedHeight(float fixedHeight)` |
| `void` | `SetFixedSize(Nz::Vector2f const& fixedSize)` |
| `void` | `SetFixedWidth(float fixedWidth)` |
| `void` | `SetMaximumHeight(float maximumHeight)` |
| `void` | `SetMaximumSize(Nz::Vector2f const& maximumSize)` |
| `void` | `SetMaximumWidth(float maximumWidth)` |
| `void` | `SetMinimumHeight(float minimumHeight)` |
| `void` | `SetMinimumSize(Nz::Vector2f const& minimumSize)` |
| `void` | `SetMinimumWidth(float minimumWidth)` |
| `void` | `SetRenderingRect(Nz::Rectf const& renderingRect)` |
| `void` | `Show(bool show)` |
| BaseWidget`&` | `operator=(`BaseWidget` const&)` |
| BaseWidget`&` | `operator=(`BaseWidget`&&)` |
