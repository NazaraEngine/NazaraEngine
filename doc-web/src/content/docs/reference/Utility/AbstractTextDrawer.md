---
title: AbstractTextDrawer
description: Nothing
---

# Nz::AbstractTextDrawer

Class description

## Constructors

- `AbstractTextDrawer()`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Clear()` |
| `Nz::Rectf const&` | `GetBounds()` |
| `std::shared_ptr<`[`Font`](documentation/generated/Utility/Font.md)`> const&` | `GetFont(std::size_t index)` |
| `std::size_t` | `GetFontCount()` |
| [`AbstractTextDrawer::Glyph`](documentation/generated/Utility/AbstractTextDrawer.Glyph.md)` const&` | `GetGlyph(std::size_t index)` |
| `std::size_t` | `GetGlyphCount()` |
| [`AbstractTextDrawer::Line`](documentation/generated/Utility/AbstractTextDrawer.Line.md)` const&` | `GetLine(std::size_t index)` |
| `std::size_t` | `GetLineCount()` |
| `std::size_t` | `GetLineGlyphCount(std::size_t index)` |
| `float` | `GetMaxLineWidth()` |
| `void` | `SetMaxLineWidth(float lineWidth)` |
