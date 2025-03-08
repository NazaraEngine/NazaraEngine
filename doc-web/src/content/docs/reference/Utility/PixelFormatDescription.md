---
title: PixelFormatDescription
description: Nothing
---

# Nz::PixelFormatDescription

Class description

## Constructors

- `PixelFormatDescription()`
- `PixelFormatDescription(Nz::PixelFormatContent formatContent, Nz::UInt8 bpp, Nz::PixelFormatSubType subType)`
- `PixelFormatDescription(std::string_view formatName, Nz::PixelFormatContent formatContent, Nz::UInt8 bpp, Nz::PixelFormatSubType subType)`
- `PixelFormatDescription(std::string_view formatName, Nz::PixelFormatContent formatContent, Bitset<> rMask, Bitset<> gMask, Bitset<> bMask, Bitset<> aMask, Nz::PixelFormatSubType subType)`
- `PixelFormatDescription(std::string_view formatName, Nz::PixelFormatContent formatContent, Nz::PixelFormatSubType rType, Bitset<> rMask, Nz::PixelFormatSubType gType, Bitset<> gMask, Nz::PixelFormatSubType bType, Bitset<> bMask, Nz::PixelFormatSubType aType, Bitset<> aMask, Nz::UInt8 bpp)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Clear()` |
| `bool` | `IsCompressed()` |
| `bool` | `IsValid()` |
| `void` | `RecomputeBitsPerPixel()` |
| `bool` | `Validate()` |
