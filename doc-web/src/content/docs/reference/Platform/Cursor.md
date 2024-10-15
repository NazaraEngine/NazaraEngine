---
title: Cursor
description: Nothing
---

# Nz::Cursor

Class description

## Constructors

- `Cursor()`
- `Cursor(`[`Image`](documentation/generated/Utility/Image.md)` const& cursor, Nz::Vector2i const& hotSpot, Nz::SystemCursor placeholder)`
- `Cursor(`Cursor` const&)`
- `Cursor(`Cursor`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Create(`[`Image`](documentation/generated/Utility/Image.md)` const& cursor, Nz::Vector2i const& hotSpot, Nz::SystemCursor placeholder)` |
| `void` | `Destroy()` |
| `Nz::SystemCursor` | `GetSystemCursor()` |
| `bool` | `IsValid()` |
| Cursor`&` | `operator=(`Cursor` const&)` |
| Cursor`&` | `operator=(`Cursor`&&)` |
