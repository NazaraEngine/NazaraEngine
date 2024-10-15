---
title: ImageWidget
description: Nothing
---

# Nz::ImageWidget

Class description

## Constructors

- `ImageWidget(`[`BaseWidget`](documentation/generated/Widgets/BaseWidget.md)`* parent, std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> material)`
- `ImageWidget(`ImageWidget` const&)`
- `ImageWidget(`ImageWidget`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`Color`](documentation/generated/Core/Color.md)` const&` | `GetColor()` |
| `std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> const&` | `GetMaterial()` |
| `Nz::Rectf const&` | `GetTextureCoords()` |
| `void` | `SetColor(`[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `SetMaterial(std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> const& texture)` |
| `void` | `SetTextureCoords(Nz::Rectf const& coords)` |
| `void` | `SetTextureRect(Nz::Rectf const& rect)` |
| ImageWidget`&` | `operator=(`ImageWidget` const&)` |
| ImageWidget`&` | `operator=(`ImageWidget`&&)` |
