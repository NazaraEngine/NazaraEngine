---
title: FontData
description: Nothing
---

# Nz::FontData

Class description

## Constructors

- `FontData()`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `ExtractGlyph(unsigned int characterSize, char32_t character, Nz::TextStyleFlags style, float outlineThickness, `[`FontGlyph`](documentation/generated/Utility/FontGlyph.md)`* dst)` |
| `std::string` | `GetFamilyName()` |
| `std::string` | `GetStyleName()` |
| `bool` | `HasKerning()` |
| `bool` | `IsScalable()` |
| `int` | `QueryKerning(unsigned int characterSize, char32_t first, char32_t second)` |
| `unsigned int` | `QueryLineHeight(unsigned int characterSize)` |
| `float` | `QueryUnderlinePosition(unsigned int characterSize)` |
| `float` | `QueryUnderlineThickness(unsigned int characterSize)` |
| `bool` | `SupportsOutline(float outlineThickness)` |
| `bool` | `SupportsStyle(Nz::TextStyleFlags style)` |
