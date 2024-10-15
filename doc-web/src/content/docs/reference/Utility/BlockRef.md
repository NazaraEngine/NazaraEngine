---
title: BlockRef
description: Nothing
---

# Nz::BlockRef

Class description

## Constructors

- `BlockRef(Nz::RichTextDrawer::BlockRef const&)`
- `BlockRef(Nz::RichTextDrawer::BlockRef&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `float` | `GetCharacterSpacingOffset()` |
| `unsigned int` | `GetCharacterSize()` |
| `Nz::Color` | `GetColor()` |
| `std::size_t` | `GetFirstGlyphIndex()` |
| `std::shared_ptr<`[`Font`](documentation/generated/Utility/Font.md)`> const&` | `GetFont()` |
| `float` | `GetLineSpacingOffset()` |
| `Nz::Color` | `GetOutlineColor()` |
| `float` | `GetOutlineThickness()` |
| `Nz::TextStyleFlags` | `GetStyle()` |
| `std::string const&` | `GetText()` |
| `void` | `SetCharacterSpacingOffset(float offset)` |
| `void` | `SetCharacterSize(unsigned int size)` |
| `void` | `SetColor(`[`Color`](documentation/generated/Core/Color.md)` color)` |
| `void` | `SetFont(std::shared_ptr<`[`Font`](documentation/generated/Utility/Font.md)`> font)` |
| `void` | `SetLineSpacingOffset(float offset)` |
| `void` | `SetOutlineColor(`[`Color`](documentation/generated/Core/Color.md)` color)` |
| `void` | `SetOutlineThickness(float thickness)` |
| `void` | `SetStyle(Nz::TextStyleFlags style)` |
| `void` | `SetText(std::string text)` |
| `Nz::RichTextDrawer::BlockRef&` | `operator=(Nz::RichTextDrawer::BlockRef const&)` |
| `Nz::RichTextDrawer::BlockRef&` | `operator=(Nz::RichTextDrawer::BlockRef&&)` |
