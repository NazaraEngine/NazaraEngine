---
title: AbstractImage
description: Nothing
---

# Nz::AbstractImage

Class description

## Constructors

- `AbstractImage()`
- `AbstractImage(`AbstractImage` const&)`
- `AbstractImage(`AbstractImage`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::UInt8` | `GetBytesPerPixel()` |
| `Nz::PixelFormat` | `GetFormat()` |
| `Nz::UInt8` | `GetLevelCount()` |
| `Nz::Vector3ui` | `GetSize(Nz::UInt8 level)` |
| `Nz::ImageType` | `GetType()` |
| `bool` | `IsCompressed()` |
| `bool` | `IsCubemap()` |
| `bool` | `Update(void const* pixels, unsigned int srcWidth, unsigned int srcHeight, Nz::UInt8 level)` |
| `bool` | `Update(void const* pixels, Nz::Boxui const& box, unsigned int srcWidth, unsigned int srcHeight, Nz::UInt8 level)` |
| `bool` | `Update(void const* pixels, Nz::Rectui const& rect, unsigned int z, unsigned int srcWidth, unsigned int srcHeight, Nz::UInt8 level)` |
| AbstractImage`&` | `operator=(`AbstractImage` const&)` |
| AbstractImage`&` | `operator=(`AbstractImage`&&)` |
