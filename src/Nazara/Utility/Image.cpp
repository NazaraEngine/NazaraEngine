// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Image.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

///TODO: Rajouter des warnings (Formats compressés avec les méthodes Copy/Update, tests taille dans Copy)
///TODO: Rendre les méthodes exception-safe (faire usage du RAII)
///FIXME: Gérer correctement les formats utilisant moins d'un octet par pixel

namespace Nz
{
	namespace
	{
		inline unsigned int GetImageLevelSize(unsigned int size, UInt8 level)
		{
			if (size == 0) // Possible dans le cas d'une image invalide
				return 0;

			return std::max(size >> level, 1U);
		}

		inline UInt8* GetPixelPtr(UInt8* base, UInt8 bpp, unsigned int x, unsigned int y, unsigned int z, unsigned int width, unsigned int height)
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

	Image::Image(ImageType type, PixelFormat format, unsigned int width, unsigned int height, unsigned int depth, UInt8 levelCount) :
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
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("image must be valid");
			return false;
		}

		if (!PixelFormatInfo::IsValid(newFormat))
		{
			NazaraError("invalid pixel format");
			return false;
		}

		if (!PixelFormatInfo::IsConversionSupported(m_sharedImage->format, newFormat))
		{
			NazaraErrorFmt("conversion from {0} to {1} is not supported", PixelFormatInfo::GetName(m_sharedImage->format), PixelFormatInfo::GetName(newFormat));
			return false;
		}
		#endif

		if (m_sharedImage->format == newFormat)
			return true;

		SharedImage::PixelContainer levels(m_sharedImage->levels.size());

		unsigned int width = m_sharedImage->width;
		unsigned int height = m_sharedImage->height;

		// Les images 3D et cubemaps sont stockés de la même façon
		unsigned int depth = (m_sharedImage->type == ImageType::Cubemap) ? 6 : m_sharedImage->depth;

		for (unsigned int i = 0; i < levels.size(); ++i)
		{
			unsigned int pixelsPerFace = width * height;
			levels[i] = std::make_unique<UInt8[]>(pixelsPerFace * depth * PixelFormatInfo::GetBytesPerPixel(newFormat));

			UInt8* dst = levels[i].get();
			UInt8* src = m_sharedImage->levels[i].get();
			unsigned int srcStride = pixelsPerFace * PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format);
			unsigned int dstStride = pixelsPerFace * PixelFormatInfo::GetBytesPerPixel(newFormat);

			for (unsigned int d = 0; d < depth; ++d)
			{
				if (!PixelFormatInfo::Convert(m_sharedImage->format, newFormat, src, &src[srcStride], dst))
				{
					NazaraError("failed to convert image");
					return false;
				}

				src += srcStride;
				dst += dstStride;
			}

			if (width > 1)
				width >>= 1;

			if (height > 1)
				height >>= 1;

			if (depth > 1 && m_sharedImage->type != ImageType::Cubemap)
				depth >>= 1;
		}

		SharedImage* newImage = new SharedImage(1, m_sharedImage->type, newFormat, std::move(levels), m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth);

		ReleaseImage();
		m_sharedImage = newImage;

		return true;
	}

	void Image::Copy(const Image& source, const Boxui& srcBox, const Vector3ui& dstPos)
	{
		NazaraAssert(IsValid(), "invalid image");
		NazaraAssert(source.IsValid(), "invalid source image");
		NazaraAssert(source.GetFormat() == m_sharedImage->format, "image formats don't match");

		const UInt8* srcPtr = source.GetConstPixels(srcBox.x, srcBox.y, srcBox.z);
		#if NAZARA_UTILITY_SAFE
		if (!srcPtr)
		{
			NazaraError("failed to access pixels");
			return;
		}
		#endif

		UInt8 bpp = PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format);
		UInt8* dstPtr = GetPixelPtr(m_sharedImage->levels[0].get(), bpp, dstPos.x, dstPos.y, dstPos.z, m_sharedImage->width, m_sharedImage->height);

		Copy(dstPtr, srcPtr, m_sharedImage->format, srcBox.width, srcBox.height, srcBox.depth, m_sharedImage->width, m_sharedImage->height, source.GetWidth(), source.GetHeight());
	}

	bool Image::Create(ImageType type, PixelFormat format, unsigned int width, unsigned int height, unsigned int depth, UInt8 levelCount)
	{
		Destroy();

		#if NAZARA_UTILITY_SAFE
		if (!PixelFormatInfo::IsValid(format))
		{
			NazaraError("invalid pixel format");
			return false;
		}

		if (width == 0)
		{
			NazaraError("width must be at least 1 (0)");
			return false;
		}

		if (height == 0)
		{
			NazaraError("height must be at least 1 (0)");
			return false;
		}

		if (depth == 0)
		{
			NazaraError("depth must be at least 1 (0)");
			return false;
		}

		switch (type)
		{
			case ImageType::E1D:
				if (height > 1)
				{
					NazaraError("1D textures must be 1 tall");
					return false;
				}

				if (depth > 1)
				{
					NazaraError("1D textures must be 1 deep");
					return false;
				}
				break;

			case ImageType::E1D_Array:
			case ImageType::E2D:
				if (depth > 1)
				{
					NazaraError("2D textures must be 1 deep");
					return false;
				}
				break;

			case ImageType::E2D_Array:
			case ImageType::E3D:
				break;

			case ImageType::Cubemap:
				if (depth > 1)
				{
					NazaraError("cubemaps must be 1 deep");
					return false;
				}

				if (width != height)
				{
					NazaraError("cubemaps must have square dimensions");
					return false;
				}
				break;

			default:
				NazaraInternalError("Image type not handled");
				return false;
		}
		#endif

		levelCount = std::min(levelCount, GetMaxLevel(type, width, height, depth));

		SharedImage::PixelContainer levels(levelCount);

		unsigned int w = width;
		unsigned int h = height;
		unsigned int d = (type == ImageType::Cubemap) ? 6 : depth;

		for (unsigned int i = 0; i < levelCount; ++i)
		{
			// Cette allocation est protégée car sa taille dépend directement de paramètres utilisateurs
			try
			{
				levels[i] = std::make_unique<UInt8[]>(PixelFormatInfo::ComputeSize(format, w, h, d));

				if (w > 1)
					w >>= 1;

				if (h > 1)
					h >>= 1;

				if (d > 1 && type != ImageType::Cubemap)
					d >>= 1;
			}
			catch (const std::exception& e)
			{
				NazaraErrorFmt("failed to allocate image's level {0} ({1})", i, e.what());
				return false;
			}
		}

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
		///FIXME: Pourquoi cette méthode alloue une nouvelle image plutôt que de remplir l'existante ?

		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("image must be valid");
			return false;
		}

		if (PixelFormatInfo::IsCompressed(m_sharedImage->format))
		{
			NazaraError("cannot access pixels from compressed image");
			return false;
		}
		#endif

		UInt8 bpp = PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format);
		StackArray<UInt8> colorBuffer = NazaraStackArrayNoInit(UInt8, bpp);

		if (!PixelFormatInfo::Convert(PixelFormat::RGBA32F, m_sharedImage->format, &color.r, &colorBuffer[0]))
		{
			NazaraErrorFmt("failed to convert RGBA32F to {0}", PixelFormatInfo::GetName(m_sharedImage->format));
			return false;
		}

		SharedImage::PixelContainer levels(m_sharedImage->levels.size());

		unsigned int width = m_sharedImage->width;
		unsigned int height = m_sharedImage->height;

		// Les images 3D et cubemaps sont stockés de la même façon
		unsigned int depth = (m_sharedImage->type == ImageType::Cubemap) ? 6 : m_sharedImage->depth;

		for (auto & level : levels)
		{
			std::size_t size = PixelFormatInfo::ComputeSize(m_sharedImage->format, width, height, depth);
			level = std::make_unique<UInt8[]>(size);

			UInt8* ptr = level.get();
			UInt8* end = &ptr[size];

			while (ptr < end)
			{
				std::memcpy(ptr, &colorBuffer[0], bpp);
				ptr += bpp;
			}

			if (width > 1U)
				width >>= 1;

			if (height > 1U)
				height >>= 1;

			if (depth > 1U && m_sharedImage->type != ImageType::Cubemap)
				depth >>= 1;
		}

		SharedImage* newImage = new SharedImage(1, m_sharedImage->type, m_sharedImage->format, std::move(levels), m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth);

		ReleaseImage();
		m_sharedImage = newImage;

		return true;
	}

	bool Image::Fill(const Color& color, const Boxui& box)
	{
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("image must be valid");
			return false;
		}

		if (PixelFormatInfo::IsCompressed(m_sharedImage->format))
		{
			NazaraError("cannot access pixels from compressed image");
			return false;
		}

		if (!box.IsValid())
		{
			NazaraError("invalid rectangle");
			return false;
		}

		if (box.x+box.width > m_sharedImage->width || box.y+box.height > m_sharedImage->height || box.z+box.depth > m_sharedImage->depth)
		{
			NazaraError("box dimensions are out of bounds");
			return false;
		}
		#endif

		EnsureOwnership();

		UInt8 bpp = PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format);
		StackArray<UInt8> colorBuffer = NazaraStackArrayNoInit(UInt8, bpp);

		if (!PixelFormatInfo::Convert(PixelFormat::RGBA32F, m_sharedImage->format, &color.r, &colorBuffer[0]))
		{
			NazaraErrorFmt("failed to convert RGBA32F to {0}", PixelFormatInfo::GetName(m_sharedImage->format));
			return false;
		}

		///FIXME: L'algorithme a du mal avec un bpp non multiple de 2
		UInt8* dstPixels = GetPixelPtr(m_sharedImage->levels[0].get(), bpp, box.x, box.y, box.z, m_sharedImage->width, m_sharedImage->height);
		unsigned int srcStride = box.width * bpp;
		unsigned int dstStride = m_sharedImage->width * bpp;
		unsigned int faceSize = dstStride * m_sharedImage->height;
		for (unsigned int z = 0; z < box.depth; ++z)
		{
			UInt8* facePixels = dstPixels;
			for (unsigned int y = 0; y < box.height; ++y)
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

	bool Image::Fill(const Color& color, const Rectui& rect, unsigned int z)
	{
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("image must be valid");
			return false;
		}

		if (PixelFormatInfo::IsCompressed(m_sharedImage->format))
		{
			NazaraError("cannot access pixels from compressed image");
			return false;
		}

		if (!rect.IsValid())
		{
			NazaraError("invalid rectangle");
			return false;
		}

		if (rect.x + rect.width > m_sharedImage->width || rect.y + rect.height > m_sharedImage->height)
		{
			NazaraError("rectangle dimensions are out of bounds");
			return false;
		}

		unsigned int depth = (m_sharedImage->type == ImageType::Cubemap) ? 6 : m_sharedImage->depth;
		if (z >= depth)
		{
			NazaraErrorFmt("z value exceeds depth ({0} >= {1})", z, depth);
			return false;
		}
		#endif

		EnsureOwnership();

		UInt8 bpp = PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format);
		StackArray<UInt8> colorBuffer = NazaraStackArrayNoInit(UInt8, bpp);

		if (!PixelFormatInfo::Convert(PixelFormat::RGBA32F, m_sharedImage->format, &color.r, &colorBuffer[0]))
		{
			NazaraErrorFmt("failed to convert RGBA32F to {0}", PixelFormatInfo::GetName(m_sharedImage->format));
			return false;
		}

		///FIXME: L'algorithme a du mal avec un bpp non multiple de 2
		UInt8* dstPixels = GetPixelPtr(m_sharedImage->levels[0].get(), bpp, rect.x, rect.y, z, m_sharedImage->width, m_sharedImage->height);
		unsigned int srcStride = rect.width * bpp;
		unsigned int dstStride = m_sharedImage->width * bpp;
		for (unsigned int y = 0; y < rect.height; ++y)
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
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("image must be valid");
			return false;
		}
		#endif

		EnsureOwnership();

		unsigned int width = m_sharedImage->width;
		unsigned int height = m_sharedImage->height;
		unsigned int depth = (m_sharedImage->type == ImageType::Cubemap) ? 6 : m_sharedImage->depth;
		for (auto& level : m_sharedImage->levels)
		{
			UInt8* ptr = level.get();
			if (!PixelFormatInfo::Flip(PixelFlipping::Horizontally, m_sharedImage->format, width, height, depth, ptr, ptr))
			{
				NazaraError("failed to flip image");
				return false;
			}

			if (width > 1U)
				width >>= 1;

			if (height > 1U)
				height >>= 1;

			if (depth > 1U && m_sharedImage->type != ImageType::Cubemap)
				depth >>= 1;
		}

		return true;
	}

	bool Image::FlipVertically()
	{
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("image must be valid");
			return false;
		}

		if (PixelFormatInfo::IsCompressed(m_sharedImage->format))
		{
			NazaraError("cannot flip compressed image");
			return false;
		}
		#endif

		EnsureOwnership();

		unsigned int width = m_sharedImage->width;
		unsigned int height = m_sharedImage->height;
		unsigned int depth = (m_sharedImage->type == ImageType::Cubemap) ? 6 : m_sharedImage->depth;
		for (auto& level : m_sharedImage->levels)
		{
			UInt8* ptr = level.get();
			if (!PixelFormatInfo::Flip(PixelFlipping::Vertically, m_sharedImage->format, width, height, depth, ptr, ptr))
			{
				NazaraError("failed to flip image");
				return false;
			}

			if (width > 1U)
				width >>= 1;

			if (height > 1U)
				height >>= 1;

			if (depth > 1U && m_sharedImage->type != ImageType::Cubemap)
				depth >>= 1;
		}

		return true;
	}

	const UInt8* Image::GetConstPixels(unsigned int x, unsigned int y, unsigned int z, UInt8 level) const
	{
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("image must be valid");
			return nullptr;
		}

		if (level >= m_sharedImage->levels.size())
		{
			NazaraErrorFmt("level out of bounds ({0} >= {1})", level, m_sharedImage->levels.size());
			return nullptr;
		}
		#endif

		unsigned int width = GetImageLevelSize(m_sharedImage->width, level);
		#if NAZARA_UTILITY_SAFE
		if (x >= width)
		{
			NazaraErrorFmt("x value exceeds width ({0} >= {1})", x, width);
			return nullptr;
		}
		#endif

		unsigned int height = GetImageLevelSize(m_sharedImage->height, level);
		#if NAZARA_UTILITY_SAFE
		if (y >= height)
		{
			NazaraErrorFmt("y value exceeds height ({0} >= {1})", y, height);
			return nullptr;
		}

		unsigned int depth = (m_sharedImage->type == ImageType::Cubemap) ? 6 : GetImageLevelSize(m_sharedImage->depth, level);
		if (z >= depth)
		{
			NazaraErrorFmt("z value exceeds depth ({0} >= {1})", z, depth);
			return nullptr;
		}
		#endif

		return GetPixelPtr(m_sharedImage->levels[level].get(), PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format), x, y, z, width, height);
	}

	unsigned int Image::GetDepth(UInt8 level) const
	{
		#if NAZARA_UTILITY_SAFE
		if (level >= m_sharedImage->levels.size())
		{
			NazaraErrorFmt("level out of bounds ({0} >= {1})", level, m_sharedImage->levels.size());
			return 0;
		}
		#endif

		return GetImageLevelSize(m_sharedImage->depth, level);
	}

	PixelFormat Image::GetFormat() const
	{
		return m_sharedImage->format;
	}

	unsigned int Image::GetHeight(UInt8 level) const
	{
		#if NAZARA_UTILITY_SAFE
		if (level >= m_sharedImage->levels.size())
		{
			NazaraErrorFmt("level out of bounds ({0} >= {1})", level, m_sharedImage->levels.size());
			return 0;
		}
		#endif

		return GetImageLevelSize(m_sharedImage->height, level);
	}

	UInt8 Image::GetLevelCount() const
	{
		return UInt8(m_sharedImage->levels.size());
	}

	UInt8 Image::GetMaxLevel() const
	{
		return GetMaxLevel(m_sharedImage->type, m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth);
	}

	std::size_t Image::GetMemoryUsage() const
	{
		unsigned int width = m_sharedImage->width;
		unsigned int height = m_sharedImage->height;
		unsigned int depth = m_sharedImage->depth;

		unsigned int size = 0;
		for (unsigned int i = 0; i < m_sharedImage->levels.size(); ++i)
		{
			size += width * height * depth;

			if (width > 1)
				width >>= 1;

			if (height > 1)
				height >>= 1;

			if (depth > 1)
				depth >>= 1;
		}

		if (m_sharedImage->type == ImageType::Cubemap)
			size *= 6;

		return size * PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format);
	}

	std::size_t Image::GetMemoryUsage(UInt8 level) const
	{
		return PixelFormatInfo::ComputeSize(m_sharedImage->format, GetImageLevelSize(m_sharedImage->width, level), GetImageLevelSize(m_sharedImage->height, level), ((m_sharedImage->type == ImageType::Cubemap) ? 6 : GetImageLevelSize(m_sharedImage->depth, level)));
	}

	Color Image::GetPixelColor(unsigned int x, unsigned int y, unsigned int z) const
	{
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("image must be valid");
			return Color();
		}

		if (PixelFormatInfo::IsCompressed(m_sharedImage->format))
		{
			NazaraError("cannot access pixels from compressed image");
			return Color();
		}

		if (x >= m_sharedImage->width)
		{
			NazaraErrorFmt("x value exceeds width ({0} >= {1})", x, m_sharedImage->width);
			return Color();
		}

		if (y >= m_sharedImage->height)
		{
			NazaraErrorFmt("y value exceeds height ({0} >= {1})", y, m_sharedImage->height);
			return Color();
		}

		unsigned int depth = (m_sharedImage->type == ImageType::Cubemap) ? 6 : m_sharedImage->depth;
		if (z >= depth)
		{
			NazaraErrorFmt("z value exceeds depth ({0} >= {1})", z, depth);
			return Color();
		}
		#endif

		const UInt8* pixel = GetPixelPtr(m_sharedImage->levels[0].get(), PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format), x, y, z, m_sharedImage->width, m_sharedImage->height);

		Color color;
		if (!PixelFormatInfo::Convert(m_sharedImage->format, PixelFormat::RGBA32F, pixel, &color.r))
			NazaraError("failed to convert image's format to RGBA8");

		return color;
	}

	UInt8* Image::GetPixels(unsigned int x, unsigned int y, unsigned int z, UInt8 level)
	{
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("image must be valid");
			return nullptr;
		}

		if (level >= m_sharedImage->levels.size())
		{
			NazaraErrorFmt("level out of bounds ({0} >= {1})", level, m_sharedImage->levels.size());
			return nullptr;
		}
		#endif

		unsigned int width = GetImageLevelSize(m_sharedImage->width, level);
		#if NAZARA_UTILITY_SAFE
		if (x >= width)
		{
			NazaraErrorFmt("x value exceeds width ({0} >= {1})", x, width);
			return nullptr;
		}
		#endif

		unsigned int height = GetImageLevelSize(m_sharedImage->height, level);
		#if NAZARA_UTILITY_SAFE
		if (y >= height)
		{
			NazaraErrorFmt("y value exceeds height ({0} >= {1})", y, height);
			return nullptr;
		}

		unsigned int depth = (m_sharedImage->type == ImageType::Cubemap) ? 6 : GetImageLevelSize(m_sharedImage->depth, level);
		if (z >= depth)
		{
			NazaraErrorFmt("z value exceeds depth ({0} >= {1})", z, depth);
			return nullptr;
		}

		if (level >= m_sharedImage->levels.size())
		{
			NazaraErrorFmt("level out of bounds ({0} >= {1})", level, m_sharedImage->levels.size());
			return nullptr;
		}
		#endif

		EnsureOwnership();

		return GetPixelPtr(m_sharedImage->levels[level].get(), PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format), x, y, z, width, height);
	}

	Vector3ui Image::GetSize(UInt8 level) const
	{
		#if NAZARA_UTILITY_SAFE
		if (level >= m_sharedImage->levels.size())
		{
			NazaraErrorFmt("level out of bounds ({0} >= {1})", level, m_sharedImage->levels.size());
			return Vector3ui::Zero();
		}
		#endif

		return Vector3ui(GetImageLevelSize(m_sharedImage->width, level), GetImageLevelSize(m_sharedImage->height, level), GetImageLevelSize(m_sharedImage->depth, level));
	}

	ImageType Image::GetType() const
	{
		return m_sharedImage->type;
	}

	unsigned int Image::GetWidth(UInt8 level) const
	{
		#if NAZARA_UTILITY_SAFE
		if (level >= m_sharedImage->levels.size())
		{
			NazaraErrorFmt("level out of bounds ({0} >= {1})", level, m_sharedImage->levels.size());
			return 0;
		}
		#endif

		return GetImageLevelSize(m_sharedImage->width, level);
	}

	bool Image::HasAlpha() const
	{
		NazaraAssert(m_sharedImage != &emptyImage, "Image must be valid");

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

	bool Image::IsValid() const
	{
		return m_sharedImage != &emptyImage;
	}

	// LoadArray
	std::shared_ptr<Image> Image::LoadFromFile(const std::filesystem::path& filePath, const ImageParams& imageParams, const Vector2ui& atlasSize)
	{
		std::shared_ptr<Image> image = Image::LoadFromFile(filePath, imageParams);
		if (!image)
		{
			NazaraError("failed to load image");
			return nullptr;
		}

		return LoadFromImage(*image, atlasSize);
	}

	std::shared_ptr<Image> Image::LoadFromImage(const Image& image, const Vector2ui& atlasSize)
	{
		NazaraAssert(image.IsValid(), "invalid image");

		#if NAZARA_UTILITY_SAFE
		if (atlasSize.x == 0)
		{
			NazaraError("atlas width must be over zero");
			return nullptr;
		}

		if (atlasSize.y == 0)
		{
			NazaraError("atlas height must be over zero");
			return nullptr;
		}
		#endif

		ImageType type = image.GetType();

		#if NAZARA_UTILITY_SAFE
		if (type != ImageType::E1D && type != ImageType::E2D)
		{
			NazaraErrorFmt("image type not handled ({0:#x})", UnderlyingCast(type));
			return nullptr;
		}
		#endif

		Vector2ui imageSize(image.GetWidth(), image.GetHeight());

		if (imageSize.x % atlasSize.x != 0)
			NazaraWarningFmt("Image width is not divisible by atlas width ({0} mod {1} != 0)", imageSize.x, atlasSize.x);

		if (imageSize.y % atlasSize.y != 0)
			NazaraWarningFmt("Image height is not divisible by atlas height ({0} mod {1} != 0)", imageSize.y, atlasSize.y);

		Vector2ui faceSize = imageSize/atlasSize;

		unsigned int layerCount = atlasSize.x*atlasSize.y;

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

		unsigned int layer = 0;
		for (unsigned int j = 0; j < atlasSize.y; ++j)
			for (unsigned int i = 0; i < atlasSize.x; ++i)
				arrayImage->Copy(image, Rectui(i*faceSize.x, j*faceSize.y, faceSize.x, faceSize.y), Vector3ui(0, 0, layer++));

		return arrayImage;
	}

	std::shared_ptr<Image> Image::LoadFromMemory(const void* data, std::size_t size, const ImageParams& imageParams, const Vector2ui& atlasSize)
	{
		std::shared_ptr<Image> image = Image::LoadFromMemory(data, size, imageParams);
		if (!image)
		{
			NazaraError("failed to load image");
			return nullptr;
		}

		return LoadFromImage(*image, atlasSize);
	}

	std::shared_ptr<Image> Image::LoadFromStream(Stream& stream, const ImageParams& imageParams, const Vector2ui& atlasSize)
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
		NazaraAssert(image.IsValid(), "invalid image");

		#if NAZARA_UTILITY_SAFE
		ImageType type = image.GetType();
		if (type != ImageType::E2D)
		{
			NazaraErrorFmt("image type not handled ({0:#x})", UnderlyingCast(type));
			return nullptr;
		}
		#endif

		unsigned int width = image.GetWidth();
		unsigned int height = image.GetHeight();
		unsigned int faceSize = (params.faceSize == 0) ? std::max(width, height)/4 : params.faceSize;

		// Sans cette vérification, celles des rectangles pourrait réussir via un overflow
		if (width < faceSize || height < faceSize)
		{
			NazaraError("image is too small for this face size");
			return nullptr;
		}

		// Calcul et vérification des surfaces
		unsigned limitX = width - faceSize;
		unsigned limitY = height - faceSize;

		Vector2ui backPos = params.backPosition * faceSize;
		if (backPos.x > limitX || backPos.y > limitY)
		{
			NazaraError("back rectangle is out of image");
			return nullptr;
		}

		Vector2ui downPos = params.downPosition * faceSize;
		if (downPos.x > limitX || downPos.y > limitY)
		{
			NazaraError("down rectangle is out of image");
			return nullptr;
		}

		Vector2ui forwardPos = params.forwardPosition * faceSize;
		if (forwardPos.x > limitX || forwardPos.y > limitY)
		{
			NazaraError("forward rectangle is out of image");
			return nullptr;
		}

		Vector2ui leftPos = params.leftPosition * faceSize;
		if (leftPos.x > limitX || leftPos.y > limitY)
		{
			NazaraError("left rectangle is out of image");
			return nullptr;
		}

		Vector2ui rightPos = params.rightPosition * faceSize;
		if (rightPos.x > limitX || rightPos.y > limitY)
		{
			NazaraError("right rectangle is out of image");
			return nullptr;
		}

		Vector2ui upPos = params.upPosition * faceSize;
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

		cubemap->Copy(image, Boxui(backPos.x, backPos.y, 0, faceSize, faceSize, 1), Vector3ui(0, 0, UnderlyingCast(CubemapFace::NegativeZ)));
		cubemap->Copy(image, Boxui(downPos.x, downPos.y, 0, faceSize, faceSize, 1), Vector3ui(0, 0, UnderlyingCast(CubemapFace::NegativeY)));
		cubemap->Copy(image, Boxui(forwardPos.x, forwardPos.y, 0, faceSize, faceSize, 1), Vector3ui(0, 0, UnderlyingCast(CubemapFace::PositiveZ)));
		cubemap->Copy(image, Boxui(leftPos.x, leftPos.y, 0, faceSize, faceSize, 1), Vector3ui(0, 0, UnderlyingCast(CubemapFace::NegativeX)));
		cubemap->Copy(image, Boxui(rightPos.x, rightPos.y, 0, faceSize, faceSize, 1), Vector3ui(0, 0, UnderlyingCast(CubemapFace::PositiveX)));
		cubemap->Copy(image, Boxui(upPos.x, upPos.y, 0, faceSize, faceSize, 1), Vector3ui(0, 0, UnderlyingCast(CubemapFace::PositiveY)));

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
		NazaraAssert(IsValid() && IsCubemap(), "Image must be a valid cubemap");

		unsigned int faceSize = GetWidth();
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

		Copy(convertedImage, Boxui(0, 0, 0, faceSize, faceSize, 1), Vector3ui(0, 0, UnderlyingCast(face)));
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
		Utility* utility = Utility::Instance();
		NazaraAssert(utility, "Utility module has not been initialized");

		return utility->GetImageSaver().SaveToFile(*this, filePath, params);
	}

	bool Image::SaveToStream(Stream& stream, std::string_view format, const ImageParams& params)
	{
		Utility* utility = Utility::Instance();
		NazaraAssert(utility, "Utility module has not been initialized");

		return utility->GetImageSaver().SaveToStream(*this, stream, format, params);
	}

	void Image::SetLevelCount(UInt8 levelCount)
	{
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("Image must be valid");
			return;
		}

		if (levelCount == 0)
		{
			NazaraError("Level count must be positive");
			return;
		}
		#endif

		levelCount = std::min(levelCount, GetMaxLevel());

		if (m_sharedImage->levels.size() == levelCount)
			return;

		EnsureOwnership();

		UInt8 oldLevelCount = UInt8(m_sharedImage->levels.size());
		UInt8 maxLevelCount = std::max(levelCount, oldLevelCount);

		m_sharedImage->levels.resize(levelCount);
		for (UInt8 i = oldLevelCount; i < maxLevelCount; ++i)
			m_sharedImage->levels[i] = std::make_unique<UInt8[]>(GetMemoryUsage(i));
	}

	bool Image::SetPixelColor(const Color& color, unsigned int x, unsigned int y, unsigned int z)
	{
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("image must be valid");
			return false;
		}

		if (PixelFormatInfo::IsCompressed(m_sharedImage->format))
		{
			NazaraError("cannot access pixels from compressed image");
			return false;
		}

		if (x >= m_sharedImage->width)
		{
			NazaraErrorFmt("x value exceeds width ({0} >= {1})", x, m_sharedImage->width);
			return false;
		}

		if (y >= m_sharedImage->height)
		{
			NazaraErrorFmt("y value exceeds height ({0} >= {1})", y, m_sharedImage->height);
			return false;
		}

		unsigned int depth = (m_sharedImage->type == ImageType::Cubemap) ? 6 : m_sharedImage->depth;
		if (z >= depth)
		{
			NazaraErrorFmt("z value exceeds depth ({0} >= {1})", z, depth);
			return false;
		}
		#endif

		UInt8* pixel = GetPixelPtr(m_sharedImage->levels[0].get(), PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format), x, y, z, m_sharedImage->width, m_sharedImage->height);

		if (!PixelFormatInfo::Convert(PixelFormat::RGBA32F, m_sharedImage->format, &color.r, pixel))
		{
			NazaraError("failed to convert RGBA8 to image's format");
			return false;
		}

		return true;
	}

	bool Image::Update(const void* pixels, const Boxui& box, unsigned int srcWidth, unsigned int srcHeight, UInt8 level)
	{
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("image must be valid");
			return false;
		}

		if (!pixels)
		{
			NazaraError("invalid pixel source");
			return false;
		}

		if (level >= m_sharedImage->levels.size())
		{
			NazaraErrorFmt("level out of bounds ({0} >= {1})", level, m_sharedImage->levels.size());
			return false;
		}
		#endif

		unsigned int width = GetImageLevelSize(m_sharedImage->width, level);
		unsigned int height = GetImageLevelSize(m_sharedImage->height, level);

		#if NAZARA_UTILITY_SAFE
		if (!box.IsValid())
		{
			NazaraError("invalid box");
			return false;
		}

		unsigned int depth = (m_sharedImage->type == ImageType::Cubemap) ? 6 : GetImageLevelSize(m_sharedImage->depth, level);
		if (box.x+box.width > width || box.y+box.height > height || box.z+box.depth > depth ||
		    (m_sharedImage->type == ImageType::Cubemap && box.depth > 1)) // We don't allow to update more than one face of the cubemap at once
		{
			NazaraError("box dimensions are out of bounds");
			return false;
		}
		#endif

		EnsureOwnership();

		UInt8 bpp = PixelFormatInfo::GetBytesPerPixel(m_sharedImage->format);
		UInt8* dstPixels = GetPixelPtr(m_sharedImage->levels[level].get(), bpp, box.x, box.y, box.z, width, height);

		Copy(dstPixels, static_cast<const UInt8*>(pixels), m_sharedImage->format,
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

	void Image::Copy(UInt8* destination, const UInt8* source, PixelFormat format, unsigned int width, unsigned int height, unsigned int depth, unsigned int dstWidth, unsigned int dstHeight, unsigned int srcWidth, unsigned int srcHeight)
	{
		#if NAZARA_UTILITY_SAFE
		if (width == 0)
			NazaraError("width must be greater than zero");
		if (height == 0)
			NazaraError("height must be greater than zero");
		if (depth == 0)
			NazaraError("depth must be greater than zero");
		#endif

		if (dstWidth == 0)
			dstWidth = width;

		if (dstHeight == 0)
			dstHeight = height;

		if (srcWidth == 0)
			srcWidth = width;

		if (srcHeight == 0)
			srcHeight = height;

		if ((height == 1 || (dstWidth == width && srcWidth == width)) && (depth == 1 || (dstHeight == height && srcHeight == height)))
			std::memcpy(destination, source, PixelFormatInfo::ComputeSize(format, width, height, depth));
		else
		{
			unsigned int bpp = PixelFormatInfo::GetBytesPerPixel(format);
			unsigned int lineStride = width * bpp;
			unsigned int dstLineStride = dstWidth * bpp;
			unsigned int dstFaceStride = dstLineStride * dstHeight;
			unsigned int srcLineStride = srcWidth * bpp;
			unsigned int srcFaceStride = srcLineStride * srcHeight;

			for (unsigned int i = 0; i < depth; ++i)
			{
				UInt8* dstFacePtr = destination;
				const UInt8* srcFacePtr = source;
				for (unsigned int y = 0; y < height; ++y)
				{
					std::memcpy(dstFacePtr, srcFacePtr, lineStride);

					dstFacePtr += dstLineStride;
					srcFacePtr += srcLineStride;
				}

				destination += dstFaceStride;
				source += srcFaceStride;
			}
		}
	}

	UInt8 Image::GetMaxLevel(unsigned int width, unsigned int height, unsigned int depth)
	{
		// Le niveau maximal est le niveau requis pour la plus grande taille
		return SafeCast<UInt8>(std::max(IntegralLog2(std::max({ width, height, depth })), 1U));
	}

	UInt8 Image::GetMaxLevel(ImageType type, unsigned int width, unsigned int height, unsigned int depth)
	{
		// Pour éviter que la profondeur ne soit comptée dans le calcul des niveaux
		switch (type)
		{
			case ImageType::E1D:
			case ImageType::E1D_Array:
				return GetMaxLevel(width, 1U, 1U);

			case ImageType::E2D:
			case ImageType::E2D_Array:
			case ImageType::Cubemap:
				return GetMaxLevel(width, height, 1U);

			case ImageType::E3D:
				return GetMaxLevel(width, height, depth);
		}

		NazaraErrorFmt("Image type not handled ({0:#x})", UnderlyingCast(type));
		return 0;
	}

	std::shared_ptr<Image> Image::LoadFromFile(const std::filesystem::path& filePath, const ImageParams& params)
	{
		Utility* utility = Utility::Instance();
		NazaraAssert(utility, "Utility module has not been initialized");

		return utility->GetImageLoader().LoadFromFile(filePath, params);
	}

	std::shared_ptr<Image> Image::LoadFromMemory(const void* data, std::size_t size, const ImageParams& params)
	{
		Utility* utility = Utility::Instance();
		NazaraAssert(utility, "Utility module has not been initialized");

		return utility->GetImageLoader().LoadFromMemory(data, size, params);
	}

	std::shared_ptr<Image> Image::LoadFromStream(Stream& stream, const ImageParams& params)
	{
		Utility* utility = Utility::Instance();
		NazaraAssert(utility, "Utility module has not been initialized");

		return utility->GetImageLoader().LoadFromStream(stream, params);
	}

	void Image::EnsureOwnership()
	{
		if (m_sharedImage == &emptyImage)
			return;

		if (m_sharedImage->refCount > 1)
		{
			SharedImage::PixelContainer levels(m_sharedImage->levels.size());
			for (std::size_t i = 0; i < levels.size(); ++i)
			{
				std::size_t size = GetMemoryUsage(SafeCast<UInt8>(i));
				levels[i] = std::make_unique<UInt8[]>(size);
				std::memcpy(levels[i].get(), m_sharedImage->levels[i].get(), size);
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
