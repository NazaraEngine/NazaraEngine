// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Image.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/PixelFormat.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <memory>

///TODO: Rajouter des warnings (Formats compressés avec les méthodes Copy/Update, tests taille dans Copy)
///TODO: Rendre les méthodes exception-safe (faire usage du RAII)
///FIXME: Gérer correctement les formats utilisant moins d'un octet par pixel

namespace Nz
{
	namespace
	{
		inline UInt8* GetPixelPtr(UInt8* base, UInt8 bpp, UInt32 x, UInt32 y, UInt32 z, UInt32 width, UInt32 height)
		{
			return &base[(width*(height*z + y) + x)*bpp];
		}
	}

	bool ImageParams::IsValid() const
	{
		return true; // Rien à vérifier
	}

	void ImageParams::Merge(const ImageParams& params)
	{
		if (loadFormat == PixelFormat::Undefined)
			loadFormat = params.loadFormat;
	}


	Image::Image() :
	m_sharedImage(&emptyImage)
	{
	}

	Image::Image(ImageType type, PixelFormat format, UInt32 width, UInt32 height, UInt32 depth, UInt8 levelCount) :
	m_sharedImage(&emptyImage)
	{
		ErrorFlags flags(ErrorMode::ThrowException);
		Create(type, format, width, height, depth, levelCount);
	}

	Image::Image(const Image& image) :
	AbstractImage(image),
	Resource(),
	m_sharedImage(image.m_sharedImage)
	{
		if (m_sharedImage != &emptyImage)
			m_sharedImage->refCount++;
	}

	Image::Image(SharedImage* sharedImage) :
	m_sharedImage(sharedImage)
	{
	}

	Image::~Image()
	{
		Destroy();
	}

	bool Image::Convert(PixelFormat newFormat)
	{
		NazaraAssertMsg(IsValid(), "invalid image");
		NazaraAssertMsg(PixelFormatInfo::IsValid(newFormat), "invalid pixel format");

		if (m_sharedImage->format == newFormat)
			return true;

		NazaraCheck(PixelFormatInfo::IsConversionSupported(m_sharedImage->format, newFormat), "conversion from %s to %s is not supported", PixelFormatInfo::GetName(m_sharedImage->format), PixelFormatInfo::GetName(newFormat));

		SharedImage::PixelContainer levels;
		levels.resize(m_sharedImage->levels.size());

		UInt32 width = m_sharedImage->width;
		UInt32 height = m_sharedImage->height;
		UInt32 depth = (m_sharedImage->type == ImageType::Cubemap) ? 6 : m_sharedImage->depth;

		bool succeeded = ImageUtils::ForEachLevel(levels.size(), m_sharedImage->type, width, height, depth, [&](UInt8 level, UInt32 width, UInt32 height, UInt32 depth)
		{
			UInt8* src = m_sharedImage->levels[level].get();
			if (!src)
				return true;

			std::size_t pixelsPerFace = static_cast<std::size_t>(width) * height;
			levels[level] = std::make_unique_for_overwrite<UInt8[]>(pixelsPerFace * depth * PixelFormatInfo::GetBytesPerPixel(newFormat));
			UInt8* dst = levels[level].get();

			std::size_t srcStride = pixelsPerFace * PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format);
			std::size_t dstStride = pixelsPerFace * PixelFormatInfo::GetBytesPerPixel(newFormat);

			for (UInt32 d = 0; d < depth; ++d)
			{
				if (!PixelFormatInfo::Convert(m_sharedImage->format, newFormat, src, &src[srcStride], dst))
				{
					NazaraError("failed to convert image");
					return false;
				}

				src += srcStride;
				dst += dstStride;
			}

			return true;
		});

		if (!succeeded)
			return false;

		SharedImage* newImage = new SharedImage(1, m_sharedImage->type, newFormat, std::move(levels), m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth);

		ReleaseImage();
		m_sharedImage = newImage;

		return true;
	}

	void Image::Copy(const Image& source, const Boxui32& srcBox, const Vector3ui32& dstPos)
	{
		NazaraAssertMsg(IsValid(), "invalid image");
		NazaraAssertMsg(source.IsValid(), "invalid source image");
		NazaraAssertMsg(source.GetFormat() == m_sharedImage->format, "image formats don't match");

		const UInt8* srcPtr = source.GetConstPixels(srcBox.x, srcBox.y, srcBox.z);
		NazaraAssertMsg(srcPtr, "failed to access pixels");

		UInt8* dstPtr = GetPixels(dstPos.x, dstPos.y, dstPos.z, 0);
		ImageUtils::Copy(dstPtr, srcPtr, m_sharedImage->format, srcBox.width, srcBox.height, srcBox.depth, m_sharedImage->width, m_sharedImage->height, source.GetWidth(), source.GetHeight());
	}

	bool Image::Create(ImageType type, PixelFormat format, UInt32 width, UInt32 height, UInt32 depth, UInt8 levelCount)
	{
		Destroy();

		NazaraAssertMsg(PixelFormatInfo::IsValid(format), "invalid pixel format");
		NazaraAssertMsg(width > 0, "width must be greater than zero");
		NazaraAssertMsg(height > 0, "height must be greater than zero");
		NazaraAssertMsg(depth > 0, "depth must be greater than zero");

		switch (type)
		{
			case ImageType::E1D:
				NazaraAssertMsg(height == 1, "1D textures height must be 1 (got %u)", height);
				NazaraAssertMsg(depth == 1, "1D textures height must be 1 (got %u)", depth);
				break;

			case ImageType::E1D_Array:
				NazaraAssertMsg(height == 1, "2D texture arrays height must be 1 (got %u)", height);
				NazaraAssertMsg(depth == 1, "2D texture arrays depth must be 1 (got %u)", depth);
				break;

			case ImageType::E2D:
				NazaraAssertMsg(depth == 1, "2D textures depth must be 1 (got %u)", depth);
				break;

			case ImageType::E2D_Array:
			case ImageType::E3D:
				break;

			case ImageType::Cubemap:
				NazaraAssertMsg(depth == 1, "cubemaps depth must be 1 (got %u)", depth);
				NazaraAssertMsg(width == height, "cubemap depth must be greater than 1 (%u)", depth);
				break;
		}

		levelCount = std::min(levelCount, ImageUtils::GetMaxLevel(type, width, height, depth));

		SharedImage::PixelContainer levels;
		levels.resize(levelCount);

		m_sharedImage = new SharedImage(1, type, format, std::move(levels), width, height, depth);

		return true;
	}

	void Image::Destroy()
	{
		if (m_sharedImage != &emptyImage)
			ReleaseImage();
	}

	bool Image::Fill(const Color& color)
	{
		// TODO: don't make a copy if we're the sole owner of the image

		NazaraAssertMsg(IsValid(), "invalid image");
		NazaraAssertMsg(!PixelFormatInfo::IsCompressed(m_sharedImage->format), "cannot access pixels from compressed image");

		UInt8 bpp = PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format);
		StackArray<UInt8> colorBuffer = NazaraStackArrayNoInit(UInt8, bpp);

		if (!PixelFormatInfo::Convert(PixelFormat::RGBA32F, m_sharedImage->format, &color.r, &colorBuffer[0]))
		{
			NazaraError("failed to convert RGBA32F to {0}", PixelFormatInfo::GetName(m_sharedImage->format));
			return false;
		}

		SharedImage::PixelContainer levels;
		levels.resize(m_sharedImage->levels.size());

		UInt32 width = m_sharedImage->width;
		UInt32 height = m_sharedImage->height;
		UInt32 depth = (m_sharedImage->type == ImageType::Cubemap) ? 6 : m_sharedImage->depth;

		ImageUtils::ForEachLevel(levels.size(), m_sharedImage->type, width, height, depth, [&](UInt8 level, UInt32 width, UInt32 height, UInt32 depth)
		{
			UInt8* ptr = levels[level].get();
			if (!ptr)
				return;

			std::size_t size = PixelFormatInfo::ComputeSize(m_sharedImage->format, width, height, depth);
			UInt8* end = &ptr[size];

			while (ptr < end)
			{
				std::memcpy(ptr, &colorBuffer[0], bpp);
				ptr += bpp;
			}
		});

		SharedImage* newImage = new SharedImage(1, m_sharedImage->type, m_sharedImage->format, std::move(levels), m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth);

		ReleaseImage();
		m_sharedImage = newImage;

		return true;
	}

	bool Image::Fill(const Color& color, const Boxui32& box)
	{
		NazaraAssertMsg(IsValid(), "invalid image");
		NazaraAssertMsg(!PixelFormatInfo::IsCompressed(m_sharedImage->format), "cannot access pixels from compressed image");
		NazaraAssertMsg(box.IsValid(), "invalid box");
		NazaraAssertMsg(box.x + box.width <= m_sharedImage->width, "box dimensions are out of bounds (x range: [%u;%u[ exceeds image width %u)", box.x, box.x + box.width, m_sharedImage->width);
		NazaraAssertMsg(box.y + box.height <= m_sharedImage->height, "box dimensions are out of bounds (y range: [%u;%u[ exceeds image height %u)", box.y, box.y + box.height, m_sharedImage->height);
		NazaraAssertMsg(box.z + box.depth <= m_sharedImage->depth, "box dimensions are out of bounds (z range: [%u;%u[ exceeds image depth %u)", box.z, box.z + box.depth, m_sharedImage->depth);

		UInt8 bpp = PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format);
		StackArray<UInt8> colorBuffer = NazaraStackArrayNoInit(UInt8, bpp);

		if (!PixelFormatInfo::Convert(PixelFormat::RGBA32F, m_sharedImage->format, &color.r, &colorBuffer[0]))
		{
			NazaraError("failed to convert RGBA32F to {0}", PixelFormatInfo::GetName(m_sharedImage->format));
			return false;
		}

		///FIXME: L'algorithme a du mal avec un bpp non multiple de 2
		UInt8* dstPixels = GetPixels(box.x, box.y, box.z, 0);
		std::size_t srcStride = static_cast<std::size_t>(box.width) * bpp;
		std::size_t dstStride = static_cast<std::size_t>(m_sharedImage->width) * bpp;
		std::size_t faceSize = dstStride * m_sharedImage->height;
		for (UInt32 z = 0; z < box.depth; ++z)
		{
			UInt8* facePixels = dstPixels;
			for (UInt32 y = 0; y < box.height; ++y)
			{
				UInt8* start = facePixels;
				UInt8* end = facePixels + srcStride;
				while (start < end)
				{
					std::memcpy(start, &colorBuffer[0], bpp);
					start += bpp;
				}

				facePixels += dstStride;
			}

			dstPixels += faceSize;
		}

		return true;
	}

	bool Image::Fill(const Color& color, const Rectui& rect, UInt32 z)
	{
		NazaraAssertMsg(IsValid(), "invalid image");
		NazaraAssertMsg(!PixelFormatInfo::IsCompressed(m_sharedImage->format), "cannot access pixels from compressed image");
		NazaraAssertMsg(rect.IsValid(), "invalid rect");
		NazaraAssertMsg(rect.x + rect.width <= m_sharedImage->width, "rect dimensions are out of bounds (x range: [%u;%u[ exceeds image width %u)", rect.x, rect.x + rect.width, m_sharedImage->width);
		NazaraAssertMsg(rect.y + rect.height <= m_sharedImage->height, "rect dimensions are out of bounds (y range: [%u;%u[ exceeds image height %u)", rect.y, rect.y + rect.height, m_sharedImage->height);
		NazaraAssertMsg(z < ((m_sharedImage->type == ImageType::Cubemap) ? 6 : m_sharedImage->depth), "z is out of range (%u >= %u)", z, (m_sharedImage->type == ImageType::Cubemap) ? 6 : m_sharedImage->depth);

		UInt8 bpp = PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format);
		StackArray<UInt8> colorBuffer = NazaraStackArrayNoInit(UInt8, bpp);

		if (!PixelFormatInfo::Convert(PixelFormat::RGBA32F, m_sharedImage->format, &color.r, &colorBuffer[0]))
		{
			NazaraError("failed to convert RGBA32F to {0}", PixelFormatInfo::GetName(m_sharedImage->format));
			return false;
		}

		///FIXME: L'algorithme a du mal avec un bpp non multiple de 2
		UInt8* dstPixels = GetPixels(rect.x, rect.y, z, 0);
		std::size_t srcStride = static_cast<std::size_t>(rect.width) * bpp;
		std::size_t dstStride = static_cast<std::size_t>(m_sharedImage->width) * bpp;
		for (UInt32 y = 0; y < rect.height; ++y)
		{
			UInt8* start = dstPixels;
			UInt8* end = dstPixels + srcStride;
			while (start < end)
			{
				std::memcpy(start, &colorBuffer[0], bpp);
				start += bpp;
			}

			dstPixels += dstStride;
		}

		return true;
	}

	bool Image::FlipHorizontally()
	{
		NazaraAssertMsg(IsValid(), "invalid image");

		EnsureOwnership();

		UInt32 width = m_sharedImage->width;
		UInt32 height = m_sharedImage->height;
		UInt32 depth = (m_sharedImage->type == ImageType::Cubemap) ? 6 : m_sharedImage->depth;

		return ImageUtils::ForEachLevel(m_sharedImage->levels.size(), m_sharedImage->type, width, height, depth, [&](UInt8 level, UInt32 width, UInt32 height, UInt32 depth)
		{
			UInt8* ptr = m_sharedImage->levels[level].get();
			if (!ptr)
				return true;

			if (!PixelFormatInfo::Flip(PixelFlipping::Horizontally, m_sharedImage->format, width, height, depth, ptr, ptr))
			{
				NazaraError("failed to flip image");
				return false;
			}

			return true;
		});
	}

	bool Image::FlipVertically()
	{
		NazaraAssertMsg(IsValid(), "invalid image");
		NazaraAssertMsg(!PixelFormatInfo::IsCompressed(m_sharedImage->format), "cannot access pixels from compressed image");

		EnsureOwnership();

		UInt32 width = m_sharedImage->width;
		UInt32 height = m_sharedImage->height;
		UInt32 depth = (m_sharedImage->type == ImageType::Cubemap) ? 6 : m_sharedImage->depth;

		return ImageUtils::ForEachLevel(m_sharedImage->levels.size(), m_sharedImage->type, width, height, depth, [&](UInt8 level, UInt32 width, UInt32 height, UInt32 depth)
		{
			UInt8* ptr = m_sharedImage->levels[level].get();
			if (!ptr)
				return true;

			if (!PixelFormatInfo::Flip(PixelFlipping::Vertically, m_sharedImage->format, width, height, depth, ptr, ptr))
			{
				NazaraError("failed to flip image");
				return false;
			}

			return true;
		});
	}

	void Image::FreeLevel(UInt8 level)
	{
		NazaraAssertMsg(IsValid(), "invalid image");
		NazaraAssertMsg(level < m_sharedImage->levels.size(), "level out of bounds (%u >= %zu)", UInt32(level), m_sharedImage->levels.size());

		if (m_sharedImage->levels[level])
		{
			EnsureOwnership();
			m_sharedImage->levels[level].reset();
		}
	}

	const UInt8* Image::GetConstPixels(UInt32 x, UInt32 y, UInt32 z, UInt8 level) const
	{
		NazaraAssertMsg(IsValid(), "invalid image");
		NazaraAssertMsg(level < m_sharedImage->levels.size(), "level out of bounds (%u >= %zu)", UInt32(level), m_sharedImage->levels.size());

		UInt32 width = ImageUtils::GetLevelSize(m_sharedImage->width, level);
		NazaraAssertMsg(x < width, "x value exceeds width (%u >= %u)", x, width);

		UInt32 height = ImageUtils::GetLevelSize(m_sharedImage->height, level);
		NazaraAssertMsg(y < height, "y value exceeds height (%u >= %u)", y, height);

		UInt32 depth = (m_sharedImage->type == ImageType::Cubemap) ? 6 : ImageUtils::GetLevelSize(m_sharedImage->depth, level);
		NazaraUnused(depth);
		NazaraAssertMsg(z < depth, "z value exceeds depth (%u >= %u)", z, depth);

		UInt8* ptr = m_sharedImage->levels[level].get();
		if NAZARA_UNLIKELY(!ptr)
			return nullptr;

		return GetPixelPtr(ptr, PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format), x, y, z, width, height);
	}

	UInt32 Image::GetDepth(UInt8 level) const
	{
		NazaraAssertMsg(level < m_sharedImage->levels.size(), "level out of bounds (%u >= %zu)", UInt32(level), m_sharedImage->levels.size());

		return ImageUtils::GetLevelSize(m_sharedImage->depth, level);
	}

	PixelFormat Image::GetFormat() const
	{
		return m_sharedImage->format;
	}

	UInt32 Image::GetHeight(UInt8 level) const
	{
		NazaraAssertMsg(level < m_sharedImage->levels.size(), "level out of bounds (%u >= %zu)", UInt32(level), m_sharedImage->levels.size());

		return ImageUtils::GetLevelSize(m_sharedImage->height, level);
	}

	UInt8 Image::GetLevelCount() const
	{
		return UInt8(m_sharedImage->levels.size());
	}

	UInt8 Image::GetMaxLevel() const
	{
		return ImageUtils::GetMaxLevel(m_sharedImage->type, m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth);
	}

	std::size_t Image::GetMemoryUsage() const
	{
		UInt32 width = m_sharedImage->width;
		UInt32 height = m_sharedImage->height;
		UInt32 depth = m_sharedImage->depth;

		std::size_t size = 0;
		ImageUtils::ForEachLevel(m_sharedImage->levels.size(), m_sharedImage->type, width, height, depth, [&](UInt8 /*level*/, UInt32 width, UInt32 height, UInt32 depth)
		{
			size += static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * static_cast<std::size_t>(depth);
		});

		if (m_sharedImage->type == ImageType::Cubemap)
			size *= 6;

		return size * PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format);
	}

	std::size_t Image::GetMemoryUsage(UInt8 level) const
	{
		return PixelFormatInfo::ComputeSize(m_sharedImage->format, ImageUtils::GetLevelSize(m_sharedImage->width, level), ImageUtils::GetLevelSize(m_sharedImage->height, level), ((m_sharedImage->type == ImageType::Cubemap) ? 6 : ImageUtils::GetLevelSize(m_sharedImage->depth, level)));
	}

	Color Image::GetPixelColor(UInt32 x, UInt32 y, UInt32 z) const
	{
		NazaraAssertMsg(IsValid(), "invalid image");
		NazaraAssertMsg(!PixelFormatInfo::IsCompressed(m_sharedImage->format), "cannot access pixels from compressed image");
		NazaraAssertMsg(x < m_sharedImage->width, "x value exceeds width (%u >= %u)", x, m_sharedImage->width);
		NazaraAssertMsg(y < m_sharedImage->height, "y value exceeds height (%u >= %u)", y, m_sharedImage->height);

		UInt32 depth = (m_sharedImage->type == ImageType::Cubemap) ? 6 : m_sharedImage->depth;
		NazaraUnused(depth);
		NazaraAssertMsg(z < depth, "z value exceeds depth (%u >= %u)", z, depth);

		UInt8* ptr = m_sharedImage->levels[0].get();
		NazaraAssertMsg(ptr, "level 0 is not allocated");
		const UInt8* pixel = GetPixelPtr(ptr, PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format), x, y, z, m_sharedImage->width, m_sharedImage->height);

		Color color;
		if (!PixelFormatInfo::Convert(m_sharedImage->format, PixelFormat::RGBA32F, pixel, &color.r))
			NazaraError("failed to convert image's format to RGBA8");

		return color;
	}

	UInt8* Image::GetPixels(UInt32 x, UInt32 y, UInt32 z, UInt8 level)
	{
		NazaraAssertMsg(IsValid(), "invalid image");
		NazaraAssertMsg(level < m_sharedImage->levels.size(), "level out of bounds (%u >= %zu)", UInt32(level), m_sharedImage->levels.size());

		UInt32 width = ImageUtils::GetLevelSize(m_sharedImage->width, level);
		NazaraAssertMsg(x < width, "x value exceeds width (%u >= %u)", x, width);

		UInt32 height = ImageUtils::GetLevelSize(m_sharedImage->height, level);
		NazaraAssertMsg(y < height, "y value exceeds height (%u >= %u)", y, height);

		UInt32 depth = (m_sharedImage->type == ImageType::Cubemap) ? 6 : ImageUtils::GetLevelSize(m_sharedImage->depth, level);
		NazaraUnused(depth);
		NazaraAssertMsg(z < depth, "z value exceeds depth (%u >= %u)", z, depth);

		NazaraAssertMsg(level < m_sharedImage->levels.size(), "level out of bounds (%u >= %zu)", UInt32(level), m_sharedImage->levels.size());

		EnsureOwnership();

		if NAZARA_UNLIKELY(!m_sharedImage->levels[level])
			m_sharedImage->levels[level] = std::make_unique_for_overwrite<UInt8[]>(GetMemoryUsage(level));

		return GetPixelPtr(m_sharedImage->levels[level].get(), PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format), x, y, z, width, height);
	}

	Vector3ui32 Image::GetSize(UInt8 level) const
	{
		NazaraAssertMsg(level < m_sharedImage->levels.size(), "level out of bounds (%u >= %zu)", UInt32(level), m_sharedImage->levels.size());

		return Vector3ui32(ImageUtils::GetLevelSize(m_sharedImage->width, level), ImageUtils::GetLevelSize(m_sharedImage->height, level), ImageUtils::GetLevelSize(m_sharedImage->depth, level));
	}

	ImageType Image::GetType() const
	{
		return m_sharedImage->type;
	}

	UInt32 Image::GetWidth(UInt8 level) const
	{
		NazaraAssertMsg(level < m_sharedImage->levels.size(), "level out of bounds (%u >= %zu)", UInt32(level), m_sharedImage->levels.size());

		return ImageUtils::GetLevelSize(m_sharedImage->width, level);
	}

	bool Image::HasAlpha() const
	{
		NazaraAssertMsg(m_sharedImage != &emptyImage, "Image must be valid");

		if (!PixelFormatInfo::HasAlpha(m_sharedImage->format))
			return false;

		if (!PixelFormatInfo::IsCompressed(m_sharedImage->format))
		{
			const PixelFormatDescription& info = PixelFormatInfo::GetInfo(m_sharedImage->format);

			Bitset<> workingBitset;
			std::size_t pixelCount = m_sharedImage->width * m_sharedImage->height * ((m_sharedImage->type == ImageType::Cubemap) ? 6 : m_sharedImage->depth);
			if (pixelCount == 0)
				return false;

			auto seq = workingBitset.Write(GetConstPixels(), info.bitsPerPixel);
			do
			{
				workingBitset &= info.alphaMask;
				if (workingBitset.Count() != info.alphaMask.Count()) //< Means that at least one bit of the alpha mask of this pixel is disabled
					return true;

				workingBitset.Clear();
				workingBitset.Write(seq, info.bitsPerPixel);
			}
			while (--pixelCount > 0);

			return false;
		}
		else
		{
			// FIXME: Currently, we assume the pixel format is already the right one
			return true;
		}
	}

	bool Image::IsLevelAllocated(UInt8 level) const
	{
		NazaraAssertMsg(IsValid(), "invalid image");
		NazaraAssertMsg(level < m_sharedImage->levels.size(), "level out of bounds (%u >= %zu)", UInt32(level), m_sharedImage->levels.size());

		return m_sharedImage->levels[level] != nullptr;
	}

	bool Image::IsValid() const
	{
		return m_sharedImage != &emptyImage;
	}

	// LoadArray
	std::shared_ptr<Image> Image::LoadFromFile(const std::filesystem::path& filePath, const ImageParams& imageParams, const Vector2ui32& atlasSize)
	{
		std::shared_ptr<Image> image = Image::LoadFromFile(filePath, imageParams);
		if (!image)
		{
			NazaraError("failed to load image");
			return nullptr;
		}

		return LoadFromImage(*image, atlasSize);
	}

	std::shared_ptr<Image> Image::LoadFromImage(const Image& image, const Vector2ui32& atlasSize)
	{
		NazaraAssertMsg(image.IsValid(), "invalid image");
		NazaraAssertMsg(atlasSize.x > 0, "atlas width must be over zero");
		NazaraAssertMsg(atlasSize.x > 0, "atlas height must be over zero");

		ImageType type = image.GetType();

		NazaraAssertMsg(type == ImageType::E1D || type == ImageType::E2D, "image type not handled ({0:#x})", UnderlyingCast(type));

		Vector2ui32 imageSize(image.GetWidth(), image.GetHeight());

		if (imageSize.x % atlasSize.x != 0)
			NazaraWarning("Image width is not divisible by atlas width ({0} mod {1} != 0)", imageSize.x, atlasSize.x);

		if (imageSize.y % atlasSize.y != 0)
			NazaraWarning("Image height is not divisible by atlas height ({0} mod {1} != 0)", imageSize.y, atlasSize.y);

		Vector2ui32 faceSize = imageSize/atlasSize;

		UInt32 layerCount = atlasSize.x * atlasSize.y;

		std::shared_ptr<Image> arrayImage = std::make_shared<Image>();
		// Selon le type de l'image de base, on va créer un array d'images 2D ou 1D
		if (type == ImageType::E2D)
			arrayImage->Create(ImageType::E2D_Array, image.GetFormat(), faceSize.x, faceSize.y, layerCount);
		else
			arrayImage->Create(ImageType::E1D_Array, image.GetFormat(), faceSize.x, layerCount);

		if (!arrayImage->IsValid())
		{
			NazaraError("failed to create image");
			return nullptr;
		}

		UInt32 layer = 0;
		for (UInt32 j = 0; j < atlasSize.y; ++j)
			for (UInt32 i = 0; i < atlasSize.x; ++i)
				arrayImage->Copy(image, Rectui(i*faceSize.x, j*faceSize.y, faceSize.x, faceSize.y), Vector3ui32(0, 0, layer++));

		return arrayImage;
	}

	std::shared_ptr<Image> Image::LoadFromMemory(const void* data, std::size_t size, const ImageParams& imageParams, const Vector2ui32& atlasSize)
	{
		std::shared_ptr<Image> image = Image::LoadFromMemory(data, size, imageParams);
		if (!image)
		{
			NazaraError("failed to load image");
			return nullptr;
		}

		return LoadFromImage(*image, atlasSize);
	}

	std::shared_ptr<Image> Image::LoadFromStream(Stream& stream, const ImageParams& imageParams, const Vector2ui32& atlasSize)
	{
		std::shared_ptr<Image> image = Image::LoadFromStream(stream, imageParams);
		if (!image)
		{
			NazaraError("failed to load image");
			return nullptr;
		}

		return LoadFromImage(*image, atlasSize);
	}

	std::shared_ptr<Image> Image::LoadFromFile(const std::filesystem::path& filePath, const ImageParams& imageParams, const CubemapParams& cubemapParams)
	{
		std::shared_ptr<Image> image = Image::LoadFromFile(filePath, imageParams);
		if (!image)
		{
			NazaraError("failed to load image");
			return nullptr;
		}

		return LoadFromImage(*image, cubemapParams);
	}

	std::shared_ptr<Image> Image::LoadFromImage(const Image& image, const CubemapParams& params)
	{
		NazaraAssertMsg(image.IsValid(), "invalid image");
		NazaraAssertMsg(image.GetType() == ImageType::E2D, "image type not handled ({0:#x})", UnderlyingCast(image.GetType()));

		UInt32 width = image.GetWidth();
		UInt32 height = image.GetHeight();
		UInt32 faceSize = (params.faceSize == 0) ? std::max(width, height) / 4 : params.faceSize;

		// Sans cette vérification, celles des rectangles pourrait réussir via un overflow
		if (width < faceSize || height < faceSize)
		{
			NazaraError("image is too small for this face size");
			return nullptr;
		}

		// Calcul et vérification des surfaces
		UInt32 limitX = width - faceSize;
		UInt32 limitY = height - faceSize;

		Vector2ui32 backPos = params.backPosition * faceSize;
		if (backPos.x > limitX || backPos.y > limitY)
		{
			NazaraError("back rectangle is out of image");
			return nullptr;
		}

		Vector2ui32 downPos = params.downPosition * faceSize;
		if (downPos.x > limitX || downPos.y > limitY)
		{
			NazaraError("down rectangle is out of image");
			return nullptr;
		}

		Vector2ui32 forwardPos = params.forwardPosition * faceSize;
		if (forwardPos.x > limitX || forwardPos.y > limitY)
		{
			NazaraError("forward rectangle is out of image");
			return nullptr;
		}

		Vector2ui32 leftPos = params.leftPosition * faceSize;
		if (leftPos.x > limitX || leftPos.y > limitY)
		{
			NazaraError("left rectangle is out of image");
			return nullptr;
		}

		Vector2ui32 rightPos = params.rightPosition * faceSize;
		if (rightPos.x > limitX || rightPos.y > limitY)
		{
			NazaraError("right rectangle is out of image");
			return nullptr;
		}

		Vector2ui32 upPos = params.upPosition * faceSize;
		if (upPos.x > limitX || upPos.y > limitY)
		{
			NazaraError("up rectangle is out of image");
			return nullptr;
		}

		std::shared_ptr<Image> cubemap = std::make_shared<Image>();
		if (!cubemap->Create(ImageType::Cubemap, image.GetFormat(), faceSize, faceSize))
		{
			NazaraError("failed to create cubemap");
			return nullptr;
		}

		if (image.IsLevelAllocated(0))
		{
			cubemap->Copy(image, Boxui32(backPos.x, backPos.y, 0, faceSize, faceSize, 1), Vector3ui32(0, 0, UnderlyingCast(CubemapFace::NegativeZ)));
			cubemap->Copy(image, Boxui32(downPos.x, downPos.y, 0, faceSize, faceSize, 1), Vector3ui32(0, 0, UnderlyingCast(CubemapFace::NegativeY)));
			cubemap->Copy(image, Boxui32(forwardPos.x, forwardPos.y, 0, faceSize, faceSize, 1), Vector3ui32(0, 0, UnderlyingCast(CubemapFace::PositiveZ)));
			cubemap->Copy(image, Boxui32(leftPos.x, leftPos.y, 0, faceSize, faceSize, 1), Vector3ui32(0, 0, UnderlyingCast(CubemapFace::NegativeX)));
			cubemap->Copy(image, Boxui32(rightPos.x, rightPos.y, 0, faceSize, faceSize, 1), Vector3ui32(0, 0, UnderlyingCast(CubemapFace::PositiveX)));
			cubemap->Copy(image, Boxui32(upPos.x, upPos.y, 0, faceSize, faceSize, 1), Vector3ui32(0, 0, UnderlyingCast(CubemapFace::PositiveY)));
		}

		return cubemap;
	}

	std::shared_ptr<Image> Image::LoadFromMemory(const void* data, std::size_t size, const ImageParams& imageParams, const CubemapParams& cubemapParams)
	{
		std::shared_ptr<Image> image = Image::LoadFromMemory(data, size, imageParams);
		if (!image)
		{
			NazaraError("failed to load image");
			return nullptr;
		}

		return LoadFromImage(*image, cubemapParams);
	}

	std::shared_ptr<Image> Image::LoadFromStream(Stream& stream, const ImageParams& imageParams, const CubemapParams& cubemapParams)
	{
		std::shared_ptr<Image> image = Image::LoadFromStream(stream, imageParams);
		if (!image)
		{
			NazaraError("failed to load image");
			return nullptr;
		}

		return LoadFromImage(*image, cubemapParams);
	}

	bool Image::LoadFaceFromFile(CubemapFace face, const std::filesystem::path& filePath, const ImageParams& params)
	{
		std::shared_ptr<Image> image = Image::LoadFromFile(filePath, params);
		if (!image)
		{
			NazaraError("failed to load image");
			return false;
		}

		return LoadFaceFromImage(face, *image);
	}

	bool Image::LoadFaceFromImage(CubemapFace face, const Image& image)
	{
		NazaraAssertMsg(IsValid() && IsCubemap(), "Image must be a valid cubemap");

		UInt32 faceSize = GetWidth();
		if (image.GetWidth() != faceSize || image.GetHeight() != faceSize)
		{
			NazaraError("image size must match cubemap face size");
			return false;
		}

		// Image is based on COW, no useless copy will be made
		Image convertedImage(image);

		if (!convertedImage.Convert(GetFormat()))
		{
			NazaraError("failed to convert image to cubemap format");
			return false;
		}

		Copy(convertedImage, Boxui32(0, 0, 0, faceSize, faceSize, 1), Vector3ui32(0, 0, UnderlyingCast(face)));
		return false;
	}

	bool Image::LoadFaceFromMemory(CubemapFace face, const void* data, std::size_t size, const ImageParams& params)
	{
		std::shared_ptr<Image> image = Image::LoadFromMemory(data, size, params);
		if (!image)
		{
			NazaraError("failed to load image");
			return false;
		}

		return LoadFaceFromImage(face, *image);
	}

	bool Image::LoadFaceFromStream(CubemapFace face, Stream& stream, const ImageParams& params)
	{
		std::shared_ptr<Image> image = Image::LoadFromStream(stream, params);
		if (!image)
		{
			NazaraError("failed to load image");
			return false;
		}

		return LoadFaceFromImage(face, *image);
	}

	bool Image::SaveToFile(const std::filesystem::path& filePath, const ImageParams& params)
	{
		Core* core = Core::Instance();
		NazaraAssertMsg(core, "Core module has not been initialized");

		return core->GetImageSaver().SaveToFile(*this, filePath, params);
	}

	bool Image::SaveToStream(Stream& stream, std::string_view format, const ImageParams& params)
	{
		Core* core = Core::Instance();
		NazaraAssertMsg(core, "Core module has not been initialized");

		return core->GetImageSaver().SaveToStream(*this, stream, format, params);
	}

	void Image::SetLevelCount(UInt8 levelCount, bool allocateLevels)
	{
		NazaraAssertMsg(IsValid(), "invalid image");
		NazaraAssertMsg(levelCount > 0, "level count must be over zero (%u)", UInt32(levelCount));

		levelCount = std::min(levelCount, GetMaxLevel());

		if (m_sharedImage->levels.size() == levelCount)
			return;

		EnsureOwnership();

		UInt8 oldLevelCount = UInt8(m_sharedImage->levels.size());
		UInt8 maxLevelCount = std::max(levelCount, oldLevelCount);

		m_sharedImage->levels.resize(levelCount);
		if (allocateLevels)
		{
			for (UInt8 i = oldLevelCount; i < maxLevelCount; ++i)
				m_sharedImage->levels[i] = std::make_unique_for_overwrite<UInt8[]>(GetMemoryUsage(i));
		}
	}

	bool Image::SetPixelColor(const Color& color, UInt32 x, UInt32 y, UInt32 z)
	{
		NazaraAssertMsg(IsValid(), "invalid image");
		NazaraAssertMsg(!PixelFormatInfo::IsCompressed(m_sharedImage->format), "cannot access pixels from compressed image");

		UInt8* pixel = GetPixels(x, y, z, 0);

		if (!PixelFormatInfo::Convert(PixelFormat::RGBA32F, m_sharedImage->format, &color.r, pixel))
		{
			NazaraError("failed to convert RGBA8 to image's format");
			return false;
		}

		return true;
	}

	bool Image::Update(const void* pixels, const Boxui32& box, UInt32 srcWidth, UInt32 srcHeight, UInt8 level)
	{
		NazaraAssertMsg(IsValid(), "invalid image");
		NazaraAssertMsg(pixels, "invalid pixel source");
		NazaraAssertMsg(level < m_sharedImage->levels.size(), "level out of bounds (%u >= %zu)", UInt32(level), m_sharedImage->levels.size());

		UInt32 width = ImageUtils::GetLevelSize(m_sharedImage->width, level);
		UInt32 height = ImageUtils::GetLevelSize(m_sharedImage->height, level);
		NazaraAssertMsg(box.IsValid(), "invalid box");
		NazaraAssertMsg(box.x + box.width <= width, "box dimensions are out of bounds (x range: [%u;%u[ exceeds image width %u)", box.x, box.x + box.width, width);
		NazaraAssertMsg(box.y + box.height <= height, "box dimensions are out of bounds (y range: [%u;%u[ exceeds image height %u)", box.y, box.y + box.height, height);
		UInt32 depth = (m_sharedImage->type == ImageType::Cubemap) ? 6 : ImageUtils::GetLevelSize(m_sharedImage->depth, level);
		NazaraUnused(depth);
		NazaraAssertMsg(box.z + box.depth <= depth, "box dimensions are out of bounds (z range: [%u;%u[ exceeds image depth %u)", box.z, box.z + box.depth, depth);

		UInt8* dstPixels = GetPixels(box.x, box.y, box.z, level);
		ImageUtils::Copy(dstPixels, static_cast<const UInt8*>(pixels), m_sharedImage->format,
		                 box.width, box.height, box.depth,
		                 width, height,
		                 srcWidth, srcHeight);

		return true;
	}

	Image& Image::operator=(const Image& image)
	{
		ReleaseImage();

		m_sharedImage = image.m_sharedImage;
		if (m_sharedImage != &emptyImage)
			m_sharedImage->refCount++;

		return *this;
	}

	std::shared_ptr<Image> Image::LoadFromFile(const std::filesystem::path& filePath, const ImageParams& params)
	{
		Core* core = Core::Instance();
		NazaraAssertMsg(core, "Core module has not been initialized");

		return core->GetImageLoader().LoadFromFile(filePath, params);
	}

	std::shared_ptr<Image> Image::LoadFromMemory(const void* data, std::size_t size, const ImageParams& params)
	{
		Core* core = Core::Instance();
		NazaraAssertMsg(core, "Core module has not been initialized");

		return core->GetImageLoader().LoadFromMemory(data, size, params);
	}

	std::shared_ptr<Image> Image::LoadFromStream(Stream& stream, const ImageParams& params)
	{
		Core* core = Core::Instance();
		NazaraAssertMsg(core, "Core module has not been initialized");

		return core->GetImageLoader().LoadFromStream(stream, params);
	}

	void Image::EnsureOwnership()
	{
		if (m_sharedImage == &emptyImage)
			return;

		if (m_sharedImage->refCount > 1)
		{
			SharedImage::PixelContainer levels;
			levels.resize(m_sharedImage->levels.size());

			for (std::size_t i = 0; i < levels.size(); ++i)
			{
				if (m_sharedImage->levels[i])
				{
					std::size_t size = GetMemoryUsage(SafeCast<UInt8>(i));

					levels[i] = std::make_unique_for_overwrite<UInt8[]>(size);
					std::memcpy(levels[i].get(), m_sharedImage->levels[i].get(), size);
				}
			}

			m_sharedImage->refCount--;
			m_sharedImage = new SharedImage(1, m_sharedImage->type, m_sharedImage->format, std::move(levels), m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth);
		}
	}

	void Image::ReleaseImage()
	{
		if (m_sharedImage == &emptyImage)
			return;

		if (--m_sharedImage->refCount == 0)
			delete m_sharedImage;

		m_sharedImage = &emptyImage;
	}

	Image::SharedImage Image::emptyImage(0, ImageType::E2D, PixelFormat::Undefined, Image::SharedImage::PixelContainer(), 0, 0, 0);
}
