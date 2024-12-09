---
title: Image
description: Nothing
---

# Nz::Image

Class description

## Constructors

- `Image()`
- `Image(Nz::ImageType type, Nz::PixelFormat format, unsigned int width, unsigned int height, unsigned int depth, Nz::UInt8 levelCount)`
- `Image(`[`Image::SharedImage`](documentation/generated/Utility/Image.SharedImage.md)`* sharedImage)`
- `Image(`Image` const& image)`
- `Image(`Image`&& image)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Convert(Nz::PixelFormat format)` |
| `void` | `Copy(`Image` const& source, Nz::Boxui const& srcBox, Nz::Vector3ui const& dstPos)` |
| `bool` | `Create(Nz::ImageType type, Nz::PixelFormat format, unsigned int width, unsigned int height, unsigned int depth, Nz::UInt8 levelCount)` |
| `void` | `Destroy()` |
| `bool` | `Fill(`[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `bool` | `Fill(`[`Color`](documentation/generated/Core/Color.md)` const& color, Nz::Boxui const& box)` |
| `bool` | `Fill(`[`Color`](documentation/generated/Core/Color.md)` const& color, Nz::Rectui const& rect, unsigned int z)` |
| `bool` | `FlipHorizontally()` |
| `bool` | `FlipVertically()` |
| `Nz::UInt8 const*` | `GetConstPixels(unsigned int x, unsigned int y, unsigned int z, Nz::UInt8 level)` |
| `unsigned int` | `GetDepth(Nz::UInt8 level)` |
| `Nz::PixelFormat` | `GetFormat()` |
| `unsigned int` | `GetHeight(Nz::UInt8 level)` |
| `Nz::UInt8` | `GetLevelCount()` |
| `Nz::UInt8` | `GetMaxLevel()` |
| `std::size_t` | `GetMemoryUsage()` |
| `std::size_t` | `GetMemoryUsage(Nz::UInt8 level)` |
| `Nz::Color` | `GetPixelColor(unsigned int x, unsigned int y, unsigned int z)` |
| `Nz::UInt8*` | `GetPixels(unsigned int x, unsigned int y, unsigned int z, Nz::UInt8 level)` |
| `Nz::Vector3ui` | `GetSize(Nz::UInt8 level)` |
| `Nz::ImageType` | `GetType()` |
| `unsigned int` | `GetWidth(Nz::UInt8 level)` |
| `bool` | `HasAlpha()` |
| `bool` | `IsValid()` |
| `bool` | `LoadFaceFromFile(Nz::CubemapFace face, std::filesystem::path const& filePath, `[`ImageParams`](documentation/generated/Utility/ImageParams.md)` const& params)` |
| `bool` | `LoadFaceFromImage(Nz::CubemapFace face, `Image` const& image)` |
| `bool` | `LoadFaceFromMemory(Nz::CubemapFace face, void const* data, std::size_t size, `[`ImageParams`](documentation/generated/Utility/ImageParams.md)` const& params)` |
| `bool` | `LoadFaceFromStream(Nz::CubemapFace face, `[`Stream`](documentation/generated/Core/Stream.md)`& stream, `[`ImageParams`](documentation/generated/Utility/ImageParams.md)` const& params)` |
| `bool` | `SaveToFile(std::filesystem::path const& filePath, `[`ImageParams`](documentation/generated/Utility/ImageParams.md)` const& params)` |
| `bool` | `SaveToStream(`[`Stream`](documentation/generated/Core/Stream.md)`& stream, std::string const& format, `[`ImageParams`](documentation/generated/Utility/ImageParams.md)` const& params)` |
| `void` | `SetLevelCount(Nz::UInt8 levelCount)` |
| `bool` | `SetPixelColor(`[`Color`](documentation/generated/Core/Color.md)` const& color, unsigned int x, unsigned int y, unsigned int z)` |
| `bool` | `Update(void const* pixels, Nz::Boxui const& box, unsigned int srcWidth, unsigned int srcHeight, Nz::UInt8 level)` |
| Image`&` | `operator=(`Image` const& image)` |
| Image`&` | `operator=(`Image`&& image)` |
