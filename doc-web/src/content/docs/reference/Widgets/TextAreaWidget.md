---
title: TextAreaWidget
description: Nothing
---

# Nz::TextAreaWidget

Class description

## Constructors

- `TextAreaWidget(`[`BaseWidget`](documentation/generated/Widgets/BaseWidget.md)`* parent)`
- `TextAreaWidget(`TextAreaWidget` const&)`
- `TextAreaWidget(`TextAreaWidget`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `AppendText(std::string const& text)` |
| `void` | `Clear()` |
| `void` | `Erase(std::size_t firstGlyph, std::size_t lastGlyph)` |
| `unsigned int` | `GetCharacterSize()` |
| `std::string const&` | `GetDisplayText()` |
| `float` | `GetCharacterSpacingOffset()` |
| `float` | `GetLineSpacingOffset()` |
| `std::string const&` | `GetText()` |
| [`Color`](documentation/generated/Core/Color.md)` const&` | `GetTextColor()` |
| `std::shared_ptr<`[`Font`](documentation/generated/Utility/Font.md)`> const&` | `GetTextFont()` |
| [`Color`](documentation/generated/Core/Color.md)` const&` | `GetTextOulineColor()` |
| `float` | `GetTextOulineThickness()` |
| `Nz::TextStyleFlags` | `GetTextStyle()` |
| `void` | `SetCharacterSize(unsigned int characterSize)` |
| `void` | `SetCharacterSpacingOffset(float offset)` |
| `void` | `SetLineSpacingOffset(float offset)` |
| `void` | `SetText(std::string const& text)` |
| `void` | `SetTextColor(`[`Color`](documentation/generated/Core/Color.md)` const& text)` |
| `void` | `SetTextFont(std::shared_ptr<`[`Font`](documentation/generated/Utility/Font.md)`> font)` |
| `void` | `SetTextOutlineColor(`[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `SetTextOutlineThickness(float thickness)` |
| `void` | `SetTextStyle(Nz::TextStyleFlags style)` |
| `void` | `Write(std::string const& text, std::size_t glyphPosition)` |
| TextAreaWidget`&` | `operator=(`TextAreaWidget` const&)` |
| TextAreaWidget`&` | `operator=(`TextAreaWidget`&&)` |
