---
title: Font
description: Nothing
---

# Nz::Font

Class description

## Constructors

- `Font()`
- `Font(`Font` const&)`
- `Font(`Font`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `ClearGlyphCache()` |
| `void` | `ClearKerningCache()` |
| `void` | `ClearSizeInfoCache()` |
| `bool` | `Create(std::unique_ptr<`[`FontData`](documentation/generated/Utility/FontData.md)`> data)` |
| `void` | `Destroy()` |
| `bool` | `ExtractGlyph(unsigned int characterSize, char32_t character, Nz::TextStyleFlags style, float outlineThickness, `[`FontGlyph`](documentation/generated/Utility/FontGlyph.md)`* glyph)` |
| `std::shared_ptr<`[`AbstractAtlas`](documentation/generated/Utility/AbstractAtlas.md)`> const&` | `GetAtlas()` |
| `std::size_t` | `GetCachedGlyphCount(unsigned int characterSize, Nz::TextStyleFlags style, float outlineThickness)` |
| `std::size_t` | `GetCachedGlyphCount()` |
| `std::string` | `GetFamilyName()` |
| `int` | `GetKerning(unsigned int characterSize, char32_t first, char32_t second)` |
| [`Font::Glyph`](documentation/generated/Utility/Font.Glyph.md)` const&` | `GetGlyph(unsigned int characterSize, Nz::TextStyleFlags style, float outlineThickness, char32_t character)` |
| `unsigned int` | `GetGlyphBorder()` |
| `unsigned int` | `GetMinimumStepSize()` |
| [`Font::SizeInfo`](documentation/generated/Utility/Font.SizeInfo.md)` const&` | `GetSizeInfo(unsigned int characterSize)` |
| `std::string` | `GetStyleName()` |
| `bool` | `IsValid()` |
| `bool` | `Precache(unsigned int characterSize, Nz::TextStyleFlags style, float outlineThickness, char32_t character)` |
| `bool` | `Precache(unsigned int characterSize, Nz::TextStyleFlags style, float outlineThickness, std::string const& characterSet)` |
| `void` | `SetAtlas(std::shared_ptr<`[`AbstractAtlas`](documentation/generated/Utility/AbstractAtlas.md)`> atlas)` |
| `void` | `SetGlyphBorder(unsigned int borderSize)` |
| `void` | `SetMinimumStepSize(unsigned int minimumStepSize)` |
| Font`&` | `operator=(`Font` const&)` |
| Font`&` | `operator=(`Font`&&)` |
