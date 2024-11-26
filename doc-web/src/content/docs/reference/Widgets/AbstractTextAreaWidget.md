---
title: AbstractTextAreaWidget
description: Nothing
---

# Nz::AbstractTextAreaWidget

Class description

## Constructors

- `AbstractTextAreaWidget(`[`BaseWidget`](documentation/generated/Widgets/BaseWidget.md)`* parent)`
- `AbstractTextAreaWidget(`AbstractTextAreaWidget` const&)`
- `AbstractTextAreaWidget(`AbstractTextAreaWidget`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Clear()` |
| `void` | `EnableLineWrap(bool enable)` |
| `void` | `EnableMultiline(bool enable)` |
| `void` | `EnableTabWriting(bool enable)` |
| `void` | `Erase(std::size_t glyphPosition)` |
| `void` | `Erase(std::size_t firstGlyph, std::size_t lastGlyph)` |
| `void` | `EraseSelection()` |
| `CharacterFilter&` | `GetCharacterFilter()` |
| `Nz::Vector2ui const&` | `GetCursorPosition()` |
| `Nz::Vector2ui` | `GetCursorPosition(std::size_t glyphIndex)` |
| `Nz::EchoMode` | `GetEchoMode()` |
| `std::size_t` | `GetGlyphIndex()` |
| `std::size_t` | `GetGlyphIndex(Nz::Vector2ui const& cursorPosition)` |
| `Nz::Vector2ui` | `GetHoveredGlyph(float x, float y)` |
| `bool` | `HasSelection()` |
| `bool` | `IsLineWrapEnabled()` |
| `bool` | `IsMultilineEnabled()` |
| `bool` | `IsReadOnly()` |
| `bool` | `IsTabWritingEnabled()` |
| `void` | `MoveCursor(int offset)` |
| `void` | `MoveCursor(Nz::Vector2i const& offset)` |
| `Nz::Vector2ui` | `NormalizeCursorPosition(Nz::Vector2ui cursorPosition)` |
| `void` | `SetCharacterFilter(Nz::AbstractTextAreaWidget::CharacterFilter filter)` |
| `void` | `SetCursorPosition(std::size_t glyphIndex)` |
| `void` | `SetCursorPosition(Nz::Vector2ui cursorPosition)` |
| `void` | `SetEchoMode(Nz::EchoMode echoMode)` |
| `void` | `SetReadOnly(bool readOnly)` |
| `void` | `SetSelection(Nz::Vector2ui fromPosition, Nz::Vector2ui toPosition)` |
| `void` | `Write(std::string const& text)` |
| `void` | `Write(std::string const& text, Nz::Vector2ui const& glyphPosition)` |
| `void` | `Write(std::string const& text, std::size_t glyphPosition)` |
| AbstractTextAreaWidget`&` | `operator=(`AbstractTextAreaWidget` const&)` |
| AbstractTextAreaWidget`&` | `operator=(`AbstractTextAreaWidget`&&)` |
