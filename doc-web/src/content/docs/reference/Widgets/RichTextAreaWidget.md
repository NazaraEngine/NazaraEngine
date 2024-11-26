---
title: RichTextAreaWidget
description: Nothing
---

# Nz::RichTextAreaWidget

Class description

## Constructors

- `RichTextAreaWidget(`[`BaseWidget`](documentation/generated/Widgets/BaseWidget.md)`* parent)`
- `RichTextAreaWidget(`RichTextAreaWidget` const&)`
- `RichTextAreaWidget(`RichTextAreaWidget`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `AppendText(std::string const& text)` |
| `void` | `Clear()` |
| `void` | `Erase(std::size_t firstGlyph, std::size_t lastGlyph)` |
| `unsigned int` | `GetCharacterSize()` |
| `float` | `GetCharacterSpacingOffset()` |
| `float` | `GetLineSpacingOffset()` |
| [`Color`](documentation/generated/Core/Color.md)` const&` | `GetTextColor()` |
| `std::shared_ptr<`[`Font`](documentation/generated/Utility/Font.md)`> const&` | `GetTextFont()` |
| [`Color`](documentation/generated/Core/Color.md)` const&` | `GetTextOutlineColor()` |
| `float` | `GetTextOutlineThickness()` |
| `Nz::TextStyleFlags` | `GetTextStyle()` |
| `void` | `SetCharacterSize(unsigned int characterSize)` |
| `void` | `SetCharacterSpacingOffset(float offset)` |
| `void` | `SetLineSpacingOffset(float offset)` |
| `void` | `SetTextColor(`[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `SetTextFont(std::shared_ptr<`[`Font`](documentation/generated/Utility/Font.md)`> font)` |
| `void` | `SetTextOutlineColor(`[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `SetTextOutlineThickness(float thickness)` |
| `void` | `SetTextStyle(Nz::TextStyleFlags style)` |
| `void` | `Write(std::string const& text, std::size_t glyphPosition)` |
| RichTextAreaWidget`&` | `operator=(`RichTextAreaWidget` const&)` |
| RichTextAreaWidget`&` | `operator=(`RichTextAreaWidget`&&)` |
