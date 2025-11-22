---
title: RichTextDrawer
description: Nothing
---

# Nz::RichTextDrawer

Class description

## Constructors

- `RichTextDrawer()`
- `RichTextDrawer(`RichTextDrawer` const& drawer)`
- `RichTextDrawer(`RichTextDrawer`&& drawer)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::RichTextDrawer::BlockRef` | `AppendText(std::string const& str, bool forceNewBlock)` |
| `void` | `Clear()` |
| `std::size_t` | `FindBlock(std::size_t glyphIndex)` |
| `unsigned int` | `GetBlockCharacterSize(std::size_t index)` |
| `float` | `GetBlockCharacterSpacingOffset(std::size_t index)` |
| [`Color`](documentation/generated/Core/Color.md)` const&` | `GetBlockColor(std::size_t index)` |
| `std::size_t` | `GetBlockCount()` |
| `std::size_t` | `GetBlockFirstGlyphIndex(std::size_t index)` |
| `std::shared_ptr<`[`Font`](documentation/generated/Utility/Font.md)`> const&` | `GetBlockFont(std::size_t index)` |
| `float` | `GetBlockLineHeight(std::size_t index)` |
| `float` | `GetBlockLineSpacingOffset(std::size_t index)` |
| [`Color`](documentation/generated/Core/Color.md)` const&` | `GetBlockOutlineColor(std::size_t index)` |
| `float` | `GetBlockOutlineThickness(std::size_t index)` |
| `Nz::TextStyleFlags` | `GetBlockStyle(std::size_t index)` |
| `std::string const&` | `GetBlockText(std::size_t index)` |
| `Nz::RichTextDrawer::BlockRef` | `GetBlock(std::size_t index)` |
| `Nz::Rectf const&` | `GetBounds()` |
| `unsigned int` | `GetDefaultCharacterSize()` |
| `float` | `GetDefaultCharacterSpacingOffset()` |
| [`Color`](documentation/generated/Core/Color.md)` const&` | `GetDefaultColor()` |
| `std::shared_ptr<`[`Font`](documentation/generated/Utility/Font.md)`> const&` | `GetDefaultFont()` |
| `float` | `GetDefaultLineSpacingOffset()` |
| [`Color`](documentation/generated/Core/Color.md)` const&` | `GetDefaultOutlineColor()` |
| `float` | `GetDefaultOutlineThickness()` |
| `Nz::TextStyleFlags` | `GetDefaultStyle()` |
| `std::shared_ptr<`[`Font`](documentation/generated/Utility/Font.md)`> const&` | `GetFont(std::size_t index)` |
| `std::size_t` | `GetFontCount()` |
| [`AbstractTextDrawer::Glyph`](documentation/generated/Utility/AbstractTextDrawer.Glyph.md)` const&` | `GetGlyph(std::size_t index)` |
| `std::size_t` | `GetGlyphCount()` |
| [`AbstractTextDrawer::Line`](documentation/generated/Utility/AbstractTextDrawer.Line.md)` const&` | `GetLine(std::size_t index)` |
| `std::size_t` | `GetLineCount()` |
| `float` | `GetMaxLineWidth()` |
| `bool` | `HasBlocks()` |
| `void` | `MergeBlocks()` |
| `void` | `RemoveBlock(std::size_t index)` |
| `void` | `SetBlockCharacterSize(std::size_t index, unsigned int characterSize)` |
| `void` | `SetBlockCharacterSpacingOffset(std::size_t index, float offset)` |
| `void` | `SetBlockColor(std::size_t index, `[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `SetBlockFont(std::size_t index, std::shared_ptr<`[`Font`](documentation/generated/Utility/Font.md)`> font)` |
| `void` | `SetBlockLineSpacingOffset(std::size_t index, float offset)` |
| `void` | `SetBlockOutlineColor(std::size_t index, `[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `SetBlockOutlineThickness(std::size_t index, float thickness)` |
| `void` | `SetBlockStyle(std::size_t index, Nz::TextStyleFlags style)` |
| `void` | `SetBlockText(std::size_t index, std::string str)` |
| `void` | `SetDefaultCharacterSize(unsigned int characterSize)` |
| `void` | `SetDefaultCharacterSpacingOffset(float offset)` |
| `void` | `SetDefaultColor(`[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `SetDefaultFont(std::shared_ptr<`[`Font`](documentation/generated/Utility/Font.md)`> const& font)` |
| `void` | `SetDefaultLineSpacingOffset(float offset)` |
| `void` | `SetDefaultOutlineColor(`[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `SetDefaultOutlineThickness(float thickness)` |
| `void` | `SetDefaultStyle(Nz::TextStyleFlags style)` |
| `void` | `SetMaxLineWidth(float lineWidth)` |
| RichTextDrawer`&` | `operator=(`RichTextDrawer` const& drawer)` |
| RichTextDrawer`&` | `operator=(`RichTextDrawer`&& drawer)` |
