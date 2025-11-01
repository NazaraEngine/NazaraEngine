---
title: ImageButtonWidget
description: Nothing
---

# Nz::ImageButtonWidget

Class description

## Constructors

- `ImageButtonWidget(`[`BaseWidget`](documentation/generated/Widgets/BaseWidget.md)`* parent, std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> material)`
- `ImageButtonWidget(`[`BaseWidget`](documentation/generated/Widgets/BaseWidget.md)`* parent, std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> material, float cornerSize, float cornerTexCoords)`
- `ImageButtonWidget(`[`BaseWidget`](documentation/generated/Widgets/BaseWidget.md)`* parent, std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> material, std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> hoveredMaterial, std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> pressedMaterial, float cornerSize, float cornerTexCoords)`
- `ImageButtonWidget(`ImageButtonWidget` const&)`
- `ImageButtonWidget(`ImageButtonWidget`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`Color`](documentation/generated/Core/Color.md)` const&` | `GetColor()` |
| `float` | `GetCornerSize()` |
| `float` | `GetCornerTexCoords()` |
| `std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> const&` | `GetHoveredMaterial()` |
| `std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> const&` | `GetMaterial()` |
| `std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> const&` | `GetPressedMaterial()` |
| `Nz::Rectf const&` | `GetTextureCoords()` |
| `void` | `SetColor(`[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `SetCorner(float size, float texcoords)` |
| `void` | `SetHoveredMaterial(std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> material)` |
| `void` | `SetMaterial(std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> material)` |
| `void` | `SetPressedMaterial(std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`> material)` |
| `void` | `SetTextureCoords(Nz::Rectf const& coords)` |
| ImageButtonWidget`&` | `operator=(`ImageButtonWidget` const&)` |
| ImageButtonWidget`&` | `operator=(`ImageButtonWidget`&&)` |
