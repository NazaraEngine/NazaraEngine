---
title: SimpleTextDrawer
description: Nothing
---

# Nz::SimpleTextDrawer

Class description

## Constructors

- `SimpleTextDrawer()`
- `SimpleTextDrawer(`SimpleTextDrawer` const& drawer)`
- `SimpleTextDrawer(`SimpleTextDrawer`&& drawer)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `AppendText(std::string_view const& str)` |
| `void` | `Clear()` |
| `Nz::Rectf const&` | `GetBounds()` |
| `float` | `GetCharacterSpacingOffset()` |
| `unsigned int` | `GetCharacterSize()` |
| [`Color`](documentation/generated/Core/Color.md)` const&` | `GetColor()` |
| `std::shared_ptr<`[`Font`](documentation/generated/Utility/Font.md)`> const&` | `GetFont()` |
| `std::shared_ptr<`[`Font`](documentation/generated/Utility/Font.md)`> const&` | `GetFont(std::size_t index)` |
| `std::size_t` | `GetFontCount()` |
| [`AbstractTextDrawer::Glyph`](documentation/generated/Utility/AbstractTextDrawer.Glyph.md)` const&` | `GetGlyph(std::size_t index)` |
| `std::size_t` | `GetGlyphCount()` |
| [`AbstractTextDrawer::Line`](documentation/generated/Utility/AbstractTextDrawer.Line.md)` const&` | `GetLine(std::size_t index)` |
| `std::size_t` | `GetLineCount()` |
| `float` | `GetLineHeight()` |
| `float` | `GetLineSpacingOffset()` |
| `float` | `GetMaxLineWidth()` |
| [`Color`](documentation/generated/Core/Color.md)` const&` | `GetOutlineColor()` |
| `float` | `GetOutlineThickness()` |
| `Nz::TextStyleFlags` | `GetStyle()` |
| `std::string const&` | `GetText()` |
| `void` | `SetCharacterSpacingOffset(float offset)` |
| `void` | `SetCharacterSize(unsigned int characterSize)` |
| `void` | `SetColor(`[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `SetFont(std::shared_ptr<`[`Font`](documentation/generated/Utility/Font.md)`> font)` |
| `void` | `SetLineSpacingOffset(float offset)` |
| `void` | `SetMaxLineWidth(float lineWidth)` |
| `void` | `SetOutlineColor(`[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `SetOutlineThickness(float thickness)` |
| `void` | `SetStyle(Nz::TextStyleFlags style)` |
| `void` | `SetText(std::string str)` |
| SimpleTextDrawer`&` | `operator=(`SimpleTextDrawer` const& drawer)` |
| SimpleTextDrawer`&` | `operator=(`SimpleTextDrawer`&& drawer)` |
