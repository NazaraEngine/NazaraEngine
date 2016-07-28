// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Image.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

///TODO: Rajouter des warnings (Formats compressés avec les méthodes Copy/Update, tests taille dans Copy)
///TODO: Rendre les méthodes exception-safe (faire usage du RAII)
///FIXME: Gérer correctement les formats utilisant moins d'un octet par pixel

namespace Nz
{
	namespace
	{
		inline unsigned int GetLevelSize(unsigned int size, UInt8 level)
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

	Image::Image() :
	m_sharedImage(&emptyImage)
	{
	}

	Image::Image(ImageType type, PixelFormatType format, unsigned int width, unsigned int height, unsigned int depth, UInt8 levelCount) :
	m_sharedImage(&emptyImage)
	{
		ErrorFlags flags(ErrorFlag_ThrowException);
		Create(type, format, width, height, depth, levelCount);
	}

	Image::Image(const Image& image) :
	RefCounted(),
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
		OnImageRelease(this);

		Destroy();
	}

	bool Image::Convert(PixelFormatType newFormat)
	{
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("Image must be valid");
			return false;
		}

		if (!PixelFormat::IsValid(newFormat))
		{
			NazaraError("Invalid pixel format");
			return false;
		}

		if (!PixelFormat::IsConversionSupported(m_sharedImage->format, newFormat))
		{
			NazaraError("Conversion from " + PixelFormat::GetName(m_sharedImage->format) + " to " + PixelFormat::GetName(newFormat) + " is not supported");
			return false;
		}
		#endif

		if (m_sharedImage->format == newFormat)
			return true;

		SharedImage::PixelContainer levels(m_sharedImage->levels.size());

		unsigned int width = m_sharedImage->width;
		unsigned int height = m_sharedImage->height;

		// Les images 3D et cubemaps sont stockés de la même façon
		unsigned int depth = (m_sharedImage->type == ImageType_Cubemap) ? 6 : m_sharedImage->depth;

		for (unsigned int i = 0; i < levels.size(); ++i)
		{
			unsigned int pixelsPerFace = width * height;
			levels[i].reset(new UInt8[pixelsPerFace * depth * PixelFormat::GetBytesPerPixel(newFormat)]);

			UInt8* dst = levels[i].get();
			UInt8* src = m_sharedImage->levels[i].get();
			unsigned int srcStride = pixelsPerFace * PixelFormat::GetBytesPerPixel(m_sharedImage->format);
			unsigned int dstStride = pixelsPerFace * PixelFormat::GetBytesPerPixel(newFormat);

			for (unsigned int d = 0; d < depth; ++d)
			{
				if (!PixelFormat::Convert(m_sharedImage->format, newFormat, src, &src[srcStride], dst))
				{
					NazaraError("Failed to convert image");
					return false;
				}

				src += srcStride;
				dst += dstStride;
			}

			if (width > 1)
				width >>= 1;

			if (height > 1)
				height >>= 1;

			if (depth > 1 && m_sharedImage->type != ImageType_Cubemap)
				depth >>= 1;
		}

		SharedImage* newImage = new SharedImage(1, m_sharedImage->type, newFormat, std::move(levels), m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth);

		ReleaseImage();
		m_sharedImage = newImage;

		return true;
	}

	void Image::Copy(const Image& source, const Boxui& srcBox, const Vector3ui& dstPos)
	{
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("Image must be valid");
			return;
		}

		if (!source.IsValid())
		{
			NazaraError("Source image must be valid");
			return;
		}

		if (source.GetFormat() != m_sharedImage->format)
		{
			NazaraError("Source image format does not match destination image format");
			return;
		}
		#endif

		const UInt8* srcPtr = source.GetConstPixels(srcBox.x, srcBox.y, srcBox.z);
		#if NAZARA_UTILITY_SAFE
		if (!srcPtr)
		{
			NazaraError("Failed to access pixels");
			return;
		}
		#endif

		UInt8 bpp = PixelFormat::GetBytesPerPixel(m_sharedImage->format);
		UInt8* dstPtr = GetPixelPtr(m_sharedImage->levels[0].get(), bpp, dstPos.x, dstPos.y, dstPos.z, m_sharedImage->width, m_sharedImage->height);

		Copy(dstPtr, srcPtr, m_sharedImage->format, srcBox.width, srcBox.height, srcBox.depth, m_sharedImage->width, m_sharedImage->height, source.GetWidth(), source.GetHeight());
	}

	bool Image::Create(ImageType type, PixelFormatType format, unsigned int width, unsigned int height, unsigned int depth, UInt8 levelCount)
	{
		Destroy();

		#if NAZARA_UTILITY_SAFE
		if (!PixelFormat::IsValid(format))
		{
			NazaraError("Invalid pixel format");
			return false;
		}

		if (width == 0)
		{
			NazaraError("Width must be at least 1 (0)");
			return false;
		}

		if (height == 0)
		{
			NazaraError("Height must be at least 1 (0)");
			return false;
		}

		if (depth == 0)
		{
			NazaraError("Depth must be at least 1 (0)");
			return false;
		}

		switch (type)
		{
			case ImageType_1D:
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

			case ImageType_1D_Array:
			case ImageType_2D:
				if (depth > 1)
				{
					NazaraError("2D textures must be 1 deep");
					return false;
				}
				break;

			case ImageType_2D_Array:
			case ImageType_3D:
				break;

			case ImageType_Cubemap:
				if (depth > 1)
				{
					NazaraError("Cubemaps must be 1 deep");
					return false;
				}

				if (width != height)
				{
					NazaraError("Cubemaps must have square dimensions");
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
		unsigned int d = (type == ImageType_Cubemap) ? 6 : depth;

		for (unsigned int i = 0; i < levelCount; ++i)
		{
			// Cette allocation est protégée car sa taille dépend directement de paramètres utilisateurs
			try
			{
				levels[i].reset(new UInt8[PixelFormat::ComputeSize(format, w, h, d)]);

				if (w > 1)
					w >>= 1;

				if (h > 1)
					h >>= 1;

				if (d > 1 && type != ImageType_Cubemap)
					d >>= 1;
			}
			catch (const std::exception& e)
			{
				NazaraError("Failed to allocate image's level " + String::Number(i) + " (" + String(e.what()) + ')');
				return false;
			}
		}

		m_sharedImage = new SharedImage(1, type, format, std::move(levels), width, height, depth);

		return true;
	}

	void Image::Destroy()
	{
		if (m_sharedImage != &emptyImage)
		{
			OnImageDestroy(this);
			ReleaseImage();
		}
	}

	bool Image::Fill(const Color& color)
	{
		///FIXME: Pourquoi cette méthode alloue une nouvelle image plutôt que de remplir l'existante ?

		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("Image must be valid");
			return false;
		}

		if (PixelFormat::IsCompressed(m_sharedImage->format))
		{
			NazaraError("Cannot access pixels from compressed image");
			return false;
		}
		#endif

		UInt8 bpp = PixelFormat::GetBytesPerPixel(m_sharedImage->format);
		std::unique_ptr<UInt8[]> colorBuffer(new UInt8[bpp]);
		if (!PixelFormat::Convert(PixelFormatType_RGBA8, m_sharedImage->format, &color.r, colorBuffer.get()))
		{
			NazaraError("Failed to convert RGBA8 to " + PixelFormat::GetName(m_sharedImage->format));
			return false;
		}

		SharedImage::PixelContainer levels(m_sharedImage->levels.size());

		unsigned int width = m_sharedImage->width;
		unsigned int height = m_sharedImage->height;

		// Les images 3D et cubemaps sont stockés de la même façon
		unsigned int depth = (m_sharedImage->type == ImageType_Cubemap) ? 6 : m_sharedImage->depth;

		for (unsigned int i = 0; i < levels.size(); ++i)
		{
			std::size_t size = PixelFormat::ComputeSize(m_sharedImage->format, width, height, depth);
			levels[i].reset(new UInt8[size]);

			UInt8* ptr = levels[i].get();
			UInt8* end = &ptr[size];

			while (ptr < end)
			{
				std::memcpy(ptr, colorBuffer.get(), bpp);
				ptr += bpp;
			}

			if (width > 1U)
				width >>= 1;

			if (height > 1U)
				height >>= 1;

			if (depth > 1U && m_sharedImage->type != ImageType_Cubemap)
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
			NazaraError("Image must be valid");
			return false;
		}

		if (PixelFormat::IsCompressed(m_sharedImage->format))
		{
			NazaraError("Cannot access pixels from compressed image");
			return false;
		}

		if (!box.IsValid())
		{
			NazaraError("Invalid rectangle");
			return false;
		}

		if (box.x+box.width > m_sharedImage->width || box.y+box.height > m_sharedImage->height || box.z+box.depth > m_sharedImage->depth)
		{
			NazaraError("Box dimensions are out of bounds");
			return false;
		}
		#endif

		EnsureOwnership();

		UInt8 bpp = PixelFormat::GetBytesPerPixel(m_sharedImage->format);
		std::unique_ptr<UInt8[]> colorBuffer(new UInt8[bpp]);
		if (!PixelFormat::Convert(PixelFormatType_RGBA8, m_sharedImage->format, &color.r, colorBuffer.get()))
		{
			NazaraError("Failed to convert RGBA8 to " + PixelFormat::GetName(m_sharedImage->format));
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
					std::memcpy(start, colorBuffer.get(), bpp);
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
			NazaraError("Image must be valid");
			return false;
		}

		if (PixelFormat::IsCompressed(m_sharedImage->format))
		{
			NazaraError("Cannot access pixels from compressed image");
			return false;
		}

		if (!rect.IsValid())
		{
			NazaraError("Invalid rectangle");
			return false;
		}

		if (rect.x + rect.width > m_sharedImage->width || rect.y + rect.height > m_sharedImage->height)
		{
			NazaraError("Rectangle dimensions are out of bounds");
			return false;
		}

		unsigned int depth = (m_sharedImage->type == ImageType_Cubemap) ? 6 : m_sharedImage->depth;
		if (z >= depth)
		{
			NazaraError("Z value exceeds depth (" + String::Number(z) + " >= " + String::Number(depth) + ')');
			return false;
		}
		#endif

		EnsureOwnership();

		UInt8 bpp = PixelFormat::GetBytesPerPixel(m_sharedImage->format);
		std::unique_ptr<UInt8[]> colorBuffer(new UInt8[bpp]);
		if (!PixelFormat::Convert(PixelFormatType_RGBA8, m_sharedImage->format, &color.r, colorBuffer.get()))
		{
			NazaraError("Failed to convert RGBA8 to " + PixelFormat::GetName(m_sharedImage->format));
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
				std::memcpy(start, colorBuffer.get(), bpp);
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
			NazaraError("Image must be valid");
			return false;
		}
		#endif

		EnsureOwnership();

		unsigned int width = m_sharedImage->width;
		unsigned int height = m_sharedImage->height;
		unsigned int depth = (m_sharedImage->type == ImageType_Cubemap) ? 6 : m_sharedImage->depth;
		for (unsigned int level = 0; level < m_sharedImage->levels.size(); ++level)
		{
			UInt8* ptr = m_sharedImage->levels[level].get();
			if (!PixelFormat::Flip(PixelFlipping_Horizontally, m_sharedImage->format, width, height, depth, ptr, ptr))
			{
				NazaraError("Failed to flip image");
				return false;
			}

			if (width > 1U)
				width >>= 1;

			if (height > 1U)
				height >>= 1;

			if (depth > 1U && m_sharedImage->type != ImageType_Cubemap)
				depth >>= 1;
		}

		return true;
	}

	bool Image::FlipVertically()
	{
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("Image must be valid");
			return false;
		}

		if (PixelFormat::IsCompressed(m_sharedImage->format))
		{
			NazaraError("Cannot flip compressed image");
			return false;
		}
		#endif

		EnsureOwnership();

		unsigned int width = m_sharedImage->width;
		unsigned int height = m_sharedImage->height;
		unsigned int depth = (m_sharedImage->type == ImageType_Cubemap) ? 6 : m_sharedImage->depth;
		for (unsigned int level = 0; level < m_sharedImage->levels.size(); ++level)
		{
			UInt8* ptr = m_sharedImage->levels[level].get();
			if (!PixelFormat::Flip(PixelFlipping_Vertically, m_sharedImage->format, width, height, depth, ptr, ptr))
			{
				NazaraError("Failed to flip image");
				return false;
			}

			if (width > 1U)
				width >>= 1;

			if (height > 1U)
				height >>= 1;

			if (depth > 1U && m_sharedImage->type != ImageType_Cubemap)
				depth >>= 1;
		}

		return true;
	}

	const UInt8* Image::GetConstPixels(unsigned int x, unsigned int y, unsigned int z, UInt8 level) const
	{
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("Image must be valid");
			return nullptr;
		}

		if (level >= m_sharedImage->levels.size())
		{
			NazaraError("Level out of bounds (" + String::Number(level) + " >= " + String::Number(m_sharedImage->levels.size()) + ')');
			return nullptr;
		}
		#endif

		unsigned int width = GetLevelSize(m_sharedImage->width, level);
		#if NAZARA_UTILITY_SAFE
		if (x >= width)
		{
			NazaraError("X value exceeds width (" + String::Number(x) + " >= " + String::Number(width) + ')');
			return nullptr;
		}
		#endif

		unsigned int height = GetLevelSize(m_sharedImage->height, level);
		#if NAZARA_UTILITY_SAFE
		if (y >= height)
		{
			NazaraError("Y value exceeds height (" + String::Number(y) + " >= " + String::Number(height) + ')');
			return nullptr;
		}

		unsigned int depth = (m_sharedImage->type == ImageType_Cubemap) ? 6 : GetLevelSize(m_sharedImage->depth, level);
		if (z >= depth)
		{
			NazaraError("Z value exceeds depth (" + String::Number(z) + " >= " + String::Number(depth) + ')');
			return nullptr;
		}
		#endif

		return GetPixelPtr(m_sharedImage->levels[level].get(), PixelFormat::GetBytesPerPixel(m_sharedImage->format), x, y, z, width, height);
	}

	unsigned int Image::GetDepth(UInt8 level) const
	{
		#if NAZARA_UTILITY_SAFE
		if (level >= m_sharedImage->levels.size())
		{
			NazaraError("Level out of bounds (" + String::Number(level) + " >= " + String::Number(m_sharedImage->levels.size()) + ')');
			return 0;
		}
		#endif

		return GetLevelSize(m_sharedImage->depth, level);
	}

	PixelFormatType Image::GetFormat() const
	{
		return m_sharedImage->format;
	}

	unsigned int Image::GetHeight(UInt8 level) const
	{
		#if NAZARA_UTILITY_SAFE
		if (level >= m_sharedImage->levels.size())
		{
			NazaraError("Level out of bounds (" + String::Number(level) + " >= " + String::Number(m_sharedImage->levels.size()) + ')');
			return 0;
		}
		#endif

		return GetLevelSize(m_sharedImage->height, level);
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

		if (m_sharedImage->type == ImageType_Cubemap)
			size *= 6;

		return size * PixelFormat::GetBytesPerPixel(m_sharedImage->format);
	}

	std::size_t Image::GetMemoryUsage(UInt8 level) const
	{
		return PixelFormat::ComputeSize(m_sharedImage->format, GetLevelSize(m_sharedImage->width, level), GetLevelSize(m_sharedImage->height, level), ((m_sharedImage->type == ImageType_Cubemap) ? 6 : GetLevelSize(m_sharedImage->depth, level)));
	}

	Color Image::GetPixelColor(unsigned int x, unsigned int y, unsigned int z) const
	{
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("Image must be valid");
			return Color();
		}

		if (PixelFormat::IsCompressed(m_sharedImage->format))
		{
			NazaraError("Cannot access pixels from compressed image");
			return Color();
		}

		if (x >= m_sharedImage->width)
		{
			NazaraError("X value exceeds width (" + String::Number(x) + " >= " + String::Number(m_sharedImage->width) + ')');
			return Color();
		}

		if (y >= m_sharedImage->height)
		{
			NazaraError("Y value exceeds height (" + String::Number(y) + " >= " + String::Number(m_sharedImage->height) + ')');
			return Color();
		}

		unsigned int depth = (m_sharedImage->type == ImageType_Cubemap) ? 6 : m_sharedImage->depth;
		if (z >= depth)
		{
			NazaraError("Z value exceeds depth (" + String::Number(z) + " >= " + String::Number(depth) + ')');
			return Color();
		}
		#endif

		const UInt8* pixel = GetPixelPtr(m_sharedImage->levels[0].get(), PixelFormat::GetBytesPerPixel(m_sharedImage->format), x, y, z, m_sharedImage->width, m_sharedImage->height);

		Color color;
		if (!PixelFormat::Convert(m_sharedImage->format, PixelFormatType_RGBA8, pixel, &color.r))
			NazaraError("Failed to convert image's format to RGBA8");

		return color;
	}

	UInt8* Image::GetPixels(unsigned int x, unsigned int y, unsigned int z, UInt8 level)
	{
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("Image must be valid");
			return nullptr;
		}

		if (level >= m_sharedImage->levels.size())
		{
			NazaraError("Level out of bounds (" + String::Number(level) + " >= " + String::Number(m_sharedImage->levels.size()) + ')');
			return nullptr;
		}
		#endif

		unsigned int width = GetLevelSize(m_sharedImage->width, level);
		#if NAZARA_UTILITY_SAFE
		if (x >= width)
		{
			NazaraError("X value exceeds width (" + String::Number(x) + " >= " + String::Number(width) + ')');
			return nullptr;
		}
		#endif

		unsigned int height = GetLevelSize(m_sharedImage->height, level);
		#if NAZARA_UTILITY_SAFE
		if (y >= height)
		{
			NazaraError("Y value exceeds height (" + String::Number(y) + " >= " + String::Number(height) + ')');
			return nullptr;
		}

		unsigned int depth = (m_sharedImage->type == ImageType_Cubemap) ? 6 : GetLevelSize(m_sharedImage->depth, level);
		if (z >= depth)
		{
			NazaraError("Z value exceeds depth (" + String::Number(z) + " >= " + String::Number(depth) + ')');
			return nullptr;
		}

		if (level >= m_sharedImage->levels.size())
		{
			NazaraError("Level out of bounds (" + String::Number(level) + " >= " + String::Number(m_sharedImage->levels.size()) + ')');
			return nullptr;
		}
		#endif

		EnsureOwnership();

		return GetPixelPtr(m_sharedImage->levels[level].get(), PixelFormat::GetBytesPerPixel(m_sharedImage->format), x, y, z, width, height);
	}

	Vector3ui Image::GetSize(UInt8 level) const
	{
		#if NAZARA_UTILITY_SAFE
		if (level >= m_sharedImage->levels.size())
		{
			NazaraError("Level out of bounds (" + String::Number(level) + " >= " + String::Number(m_sharedImage->levels.size()) + ')');
			return Vector3ui::Zero();
		}
		#endif

		return Vector3ui(GetLevelSize(m_sharedImage->width, level), GetLevelSize(m_sharedImage->height, level), GetLevelSize(m_sharedImage->depth, level));
	}

	ImageType	Image::GetType() const
	{
		return m_sharedImage->type;
	}

	unsigned int Image::GetWidth(UInt8 level) const
	{
		#if NAZARA_UTILITY_SAFE
		if (level >= m_sharedImage->levels.size())
		{
			NazaraError("Level out of bounds (" + String::Number(level) + " >= " + String::Number(m_sharedImage->levels.size()) + ')');
			return 0;
		}
		#endif

		return GetLevelSize(m_sharedImage->width, level);
	}

	bool Image::IsValid() const
	{
		return m_sharedImage != &emptyImage;
	}

	bool Image::LoadFromFile(const String& filePath, const ImageParams& params)
	{
		return ImageLoader::LoadFromFile(this, filePath, params);
	}

	bool Image::LoadFromMemory(const void* data, std::size_t size, const ImageParams& params)
	{
		return ImageLoader::LoadFromMemory(this, data, size, params);
	}

	bool Image::LoadFromStream(Stream& stream, const ImageParams& params)
	{
		return ImageLoader::LoadFromStream(this, stream, params);
	}

	// LoadArray
	bool Image::LoadArrayFromFile(const String& filePath, const ImageParams& imageParams, const Vector2ui& atlasSize)
	{
		Image image;
		if (!image.LoadFromFile(filePath, imageParams))
		{
			NazaraError("Failed to load image");
			return false;
		}

		return LoadArrayFromImage(image, atlasSize);
	}

	bool Image::LoadArrayFromImage(const Image& image, const Vector2ui& atlasSize)
	{
		#if NAZARA_UTILITY_SAFE
		if (!image.IsValid())
		{
			NazaraError("Image must be valid");
			return false;
		}

		if (atlasSize.x == 0)
		{
			NazaraError("Atlas width must be over zero");
			return false;
		}

		if (atlasSize.y == 0)
		{
			NazaraError("Atlas height must be over zero");
			return false;
		}
		#endif

		ImageType type = image.GetType();

		#if NAZARA_UTILITY_SAFE
		if (type != ImageType_1D && type != ImageType_2D)
		{
			NazaraError("Image type not handled (0x" + String::Number(type, 16) + ')');
			return false;
		}
		#endif

		Vector2ui imageSize(image.GetWidth(), image.GetHeight());

		if (imageSize.x % atlasSize.x != 0)
		{
			NazaraWarning("Image width is not divisible by atlas width (" + String::Number(imageSize.x) + " mod " + String::Number(atlasSize.x) + " != 0)");
		}

		if (imageSize.y % atlasSize.y != 0)
		{
			NazaraWarning("Image height is not divisible by atlas height (" + String::Number(imageSize.y) + " mod " + String::Number(atlasSize.y) + " != 0)");
		}

		Vector2ui faceSize = imageSize/atlasSize;

		unsigned int layerCount = atlasSize.x*atlasSize.y;

		// Selon le type de l'image de base, on va créer un array d'images 2D ou 1D
		if (type == ImageType_2D)
			Create(ImageType_2D_Array, image.GetFormat(), faceSize.x, faceSize.y, layerCount);
		else
			Create(ImageType_1D_Array, image.GetFormat(), faceSize.x, layerCount);

		unsigned int layer = 0;
		for (unsigned int j = 0; j < atlasSize.y; ++j)
			for (unsigned int i = 0; i < atlasSize.x; ++i)
				Copy(image, Rectui(i*faceSize.x, j*faceSize.y, faceSize.x, faceSize.y), Vector3ui(0, 0, layer++));

		return true;
	}

	bool Image::LoadArrayFromMemory(const void* data, std::size_t size, const ImageParams& imageParams, const Vector2ui& atlasSize)
	{
		Image image;
		if (!image.LoadFromMemory(data, size, imageParams))
		{
			NazaraError("Failed to load image");
			return false;
		}

		return LoadArrayFromImage(image, atlasSize);
	}

	bool Image::LoadArrayFromStream(Stream& stream, const ImageParams& imageParams, const Vector2ui& atlasSize)
	{
		Image image;
		if (!image.LoadFromStream(stream, imageParams))
		{
			NazaraError("Failed to load image");
			return false;
		}

		return LoadArrayFromImage(image, atlasSize);
	}

	bool Image::LoadCubemapFromFile(const String& filePath, const ImageParams& imageParams, const CubemapParams& cubemapParams)
	{
		Image image;
		if (!image.LoadFromFile(filePath, imageParams))
		{
			NazaraError("Failed to load image");
			return false;
		}

		return LoadCubemapFromImage(image, cubemapParams);
	}

	bool Image::LoadCubemapFromImage(const Image& image, const CubemapParams& params)
	{
		#if NAZARA_UTILITY_SAFE
		if (!image.IsValid())
		{
			NazaraError("Image must be valid");
			return false;
		}

		ImageType type = image.GetType();
		if (type != ImageType_2D)
		{
			NazaraError("Image type not handled (0x" + String::Number(type, 16) + ')');
			return false;
		}
		#endif

		unsigned int width = image.GetWidth();
		unsigned int height = image.GetHeight();
		unsigned int faceSize = (params.faceSize == 0) ? std::max(width, height)/4 : params.faceSize;

		// Sans cette vérification, celles des rectangles pourrait réussir via un overflow
		if (width < faceSize || height < faceSize)
		{
			NazaraError("Image is too small for this face size");
			return false;
		}

		// Calcul et vérification des surfaces
		unsigned limitX = width - faceSize;
		unsigned limitY = height - faceSize;

		Vector2ui backPos = params.backPosition * faceSize;
		if (backPos.x > limitX || backPos.y > limitY)
		{
			NazaraError("Back rectangle is out of image");
			return false;
		}

		Vector2ui downPos = params.downPosition * faceSize;
		if (downPos.x > limitX || downPos.y > limitY)
		{
			NazaraError("Down rectangle is out of image");
			return false;
		}

		Vector2ui forwardPos = params.forwardPosition * faceSize;
		if (forwardPos.x > limitX || forwardPos.y > limitY)
		{
			NazaraError("Forward rectangle is out of image");
			return false;
		}

		Vector2ui leftPos = params.leftPosition * faceSize;
		if (leftPos.x > limitX || leftPos.y > limitY)
		{
			NazaraError("Left rectangle is out of image");
			return false;
		}

		Vector2ui rightPos = params.rightPosition * faceSize;
		if (rightPos.x > limitX || rightPos.y > limitY)
		{
			NazaraError("Right rectangle is out of image");
			return false;
		}

		Vector2ui upPos = params.upPosition * faceSize;
		if (upPos.x > limitX || upPos.y > limitY)
		{
			NazaraError("Up rectangle is out of image");
			return false;
		}

		Create(ImageType_Cubemap, image.GetFormat(), faceSize, faceSize);

		#ifdef NAZARA_DEBUG
		// Les paramètres sont valides, que Create ne fonctionne pas relèverait d'un bug
		if (m_sharedImage == &emptyImage)
		{
			NazaraInternalError("Failed to create cubemap");
			return false;
		}
		#endif

		Copy(image, Rectui(backPos.x, backPos.y, faceSize, faceSize), Vector3ui(0, 0, CubemapFace_NegativeZ));
		Copy(image, Rectui(downPos.x, downPos.y, faceSize, faceSize), Vector3ui(0, 0, CubemapFace_NegativeY));
		Copy(image, Rectui(forwardPos.x, forwardPos.y, faceSize, faceSize), Vector3ui(0, 0, CubemapFace_PositiveZ));
		Copy(image, Rectui(leftPos.x, leftPos.y, faceSize, faceSize), Vector3ui(0, 0, CubemapFace_NegativeX));
		Copy(image, Rectui(rightPos.x, rightPos.y, faceSize, faceSize), Vector3ui(0, 0, CubemapFace_PositiveX));
		Copy(image, Rectui(upPos.x, upPos.y, faceSize, faceSize), Vector3ui(0, 0, CubemapFace_PositiveY));

		return true;
	}

	bool Image::LoadCubemapFromMemory(const void* data, std::size_t size, const ImageParams& imageParams, const CubemapParams& cubemapParams)
	{
		Image image;
		if (!image.LoadFromMemory(data, size, imageParams))
		{
			NazaraError("Failed to load image");
			return false;
		}

		return LoadCubemapFromImage(image, cubemapParams);
	}

	bool Image::LoadCubemapFromStream(Stream& stream, const ImageParams& imageParams, const CubemapParams& cubemapParams)
	{
		Image image;
		if (!image.LoadFromStream(stream, imageParams))
		{
			NazaraError("Failed to load image");
			return false;
		}

		return LoadCubemapFromImage(image, cubemapParams);
	}

	bool Image::LoadFaceFromFile(CubemapFace face, const String& filePath, const ImageParams& params)
	{
		NazaraAssert(IsValid() && IsCubemap(), "Texture must be a valid cubemap");

		Image image;
		if (!image.LoadFromFile(filePath, params))
		{
			NazaraError("Failed to load image");
			return false;
		}

		if (!image.Convert(GetFormat()))
		{
			NazaraError("Failed to convert image to texture format");
			return false;
		}

		unsigned int faceSize = GetWidth();
		if (image.GetWidth() != faceSize || image.GetHeight() != faceSize)
		{
			NazaraError("Image size must match texture face size");
			return false;
		}

		Copy(image, Rectui(0, 0, faceSize, faceSize), Vector3ui(0, 0, face));
		return true;
	}

	bool Image::LoadFaceFromMemory(CubemapFace face, const void* data, std::size_t size, const ImageParams& params)
	{
		NazaraAssert(IsValid() && IsCubemap(), "Texture must be a valid cubemap");

		Image image;
		if (!image.LoadFromMemory(data, size, params))
		{
			NazaraError("Failed to load image");
			return false;
		}

		if (!image.Convert(GetFormat()))
		{
			NazaraError("Failed to convert image to texture format");
			return false;
		}

		unsigned int faceSize = GetWidth();
		if (image.GetWidth() != faceSize || image.GetHeight() != faceSize)
		{
			NazaraError("Image size must match texture face size");
			return false;
		}

		Copy(image, Rectui(0, 0, faceSize, faceSize), Vector3ui(0, 0, face));
		return true;
	}

	bool Image::LoadFaceFromStream(CubemapFace face, Stream& stream, const ImageParams& params)
	{
		NazaraAssert(IsValid() && IsCubemap(), "Texture must be a valid cubemap");

		Image image;
		if (!image.LoadFromStream(stream, params))
		{
			NazaraError("Failed to load image");
			return false;
		}

		if (!image.Convert(GetFormat()))
		{
			NazaraError("Failed to convert image to texture format");
			return false;
	}

		unsigned int faceSize = GetWidth();
		if (image.GetWidth() != faceSize || image.GetHeight() != faceSize)
		{
			NazaraError("Image size must match texture face size");
			return false;
		}

		Copy(image, Rectui(0, 0, faceSize, faceSize), Vector3ui(0, 0, face));
		return true;
	}

	bool Image::SaveToFile(const String& filePath, const ImageParams& params)
	{
		return ImageSaver::SaveToFile(*this, filePath, params);
	}

	bool Image::SaveToStream(Stream& stream, const String& format, const ImageParams& params)
	{
		return ImageSaver::SaveToStream(*this, stream, format, params);
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
			m_sharedImage->levels[i].reset(new UInt8[GetMemoryUsage(i)]);
	}

	bool Image::SetPixelColor(const Color& color, unsigned int x, unsigned int y, unsigned int z)
	{
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("Image must be valid");
			return false;
		}

		if (PixelFormat::IsCompressed(m_sharedImage->format))
		{
			NazaraError("Cannot access pixels from compressed image");
			return false;
		}

		if (x >= m_sharedImage->width)
		{
			NazaraError("X value exceeds width (" + String::Number(x) + " >= " + String::Number(m_sharedImage->width) + ')');
			return false;
		}

		if (y >= m_sharedImage->height)
		{
			NazaraError("Y value exceeds height (" + String::Number(y) + " >= " + String::Number(m_sharedImage->height) + ')');
			return false;
		}

		unsigned int depth = (m_sharedImage->type == ImageType_Cubemap) ? 6 : m_sharedImage->depth;
		if (z >= depth)
		{
			NazaraError("Z value exceeds depth (" + String::Number(z) + " >= " + String::Number(depth) + ')');
			return false;
		}
		#endif

		UInt8* pixel = GetPixelPtr(m_sharedImage->levels[0].get(), PixelFormat::GetBytesPerPixel(m_sharedImage->format), x, y, z, m_sharedImage->width, m_sharedImage->height);

		if (!PixelFormat::Convert(PixelFormatType_RGBA8, m_sharedImage->format, &color.r, pixel))
		{
			NazaraError("Failed to convert RGBA8 to image's format");
			return false;
		}

		return true;
	}

	bool Image::Update(const UInt8* pixels, unsigned int srcWidth, unsigned int srcHeight, UInt8 level)
	{
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("Image must be valid");
			return false;
		}

		if (!pixels)
		{
			NazaraError("Invalid pixel source");
			return false;
		}

		if (level >= m_sharedImage->levels.size())
		{
			NazaraError("Level out of bounds (" + String::Number(level) + " >= " + String::Number(m_sharedImage->levels.size()) + ')');
			return false;
		}
		#endif

		EnsureOwnership();

		Copy(m_sharedImage->levels[level].get(), pixels, m_sharedImage->format,
		     GetLevelSize(m_sharedImage->width, level),
		     GetLevelSize(m_sharedImage->height, level),
		     GetLevelSize(m_sharedImage->depth, level),
		     0, 0,
		     srcWidth, srcHeight);

		return true;
	}

	bool Image::Update(const UInt8* pixels, const Boxui& box, unsigned int srcWidth, unsigned int srcHeight, UInt8 level)
	{
		#if NAZARA_UTILITY_SAFE
		if (m_sharedImage == &emptyImage)
		{
			NazaraError("Image must be valid");
			return false;
		}

		if (!pixels)
		{
			NazaraError("Invalid pixel source");
			return false;
		}

		if (level >= m_sharedImage->levels.size())
		{
			NazaraError("Level out of bounds (" + String::Number(level) + " >= " + String::Number(m_sharedImage->levels.size()) + ')');
			return false;
		}
		#endif

		unsigned int width = GetLevelSize(m_sharedImage->width, level);
		unsigned int height = GetLevelSize(m_sharedImage->height, level);

		#if NAZARA_UTILITY_SAFE
		if (!box.IsValid())
		{
			NazaraError("Invalid box");
			return false;
		}

		unsigned int depth = (m_sharedImage->type == ImageType_Cubemap) ? 6 : GetLevelSize(m_sharedImage->depth, level);
		if (box.x+box.width > width || box.y+box.height > height || box.z+box.depth > depth ||
			(m_sharedImage->type == ImageType_Cubemap && box.depth > 1)) // Nous n'autorisons pas de modifier plus d'une face du cubemap à la fois
		{
			NazaraError("Box dimensions are out of bounds");
			return false;
		}
		#endif

		EnsureOwnership();

		UInt8 bpp = PixelFormat::GetBytesPerPixel(m_sharedImage->format);
		UInt8* dstPixels = GetPixelPtr(m_sharedImage->levels[level].get(), bpp, box.x, box.y, box.z, width, height);

		Copy(dstPixels, pixels, m_sharedImage->format,
			 box.width, box.height, box.depth,
			 width, height,
			 srcWidth, srcHeight);

		return true;
	}

	bool Image::Update(const UInt8* pixels, const Rectui& rect, unsigned int z, unsigned int srcWidth, unsigned int srcHeight, UInt8 level)
	{
		return Update(pixels, Boxui(rect.x, rect.y, z, rect.width, rect.height, 1), srcWidth, srcHeight, level);
	}

	Image& Image::operator=(const Image& image)
	{
		ReleaseImage();

		m_sharedImage = image.m_sharedImage;
		if (m_sharedImage != &emptyImage)
			m_sharedImage->refCount++;

		return *this;
	}

	void Image::Copy(UInt8* destination, const UInt8* source, PixelFormatType format, unsigned int width, unsigned int height, unsigned int depth, unsigned int dstWidth, unsigned int dstHeight, unsigned int srcWidth, unsigned int srcHeight)
	{
		#if NAZARA_UTILITY_SAFE
		if (width == 0)
			NazaraError("Width must be greater than zero");
		if (height == 0)
			NazaraError("Height must be greater than zero");
		if (depth == 0)
			NazaraError("Depth must be greater than zero");
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
			std::memcpy(destination, source, PixelFormat::ComputeSize(format, width, height, depth));
		else
		{
			unsigned int bpp = PixelFormat::GetBytesPerPixel(format);
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
		return std::max(IntegralLog2(std::max({width, height, depth})), 1U);
	}

	UInt8 Image::GetMaxLevel(ImageType type, unsigned int width, unsigned int height, unsigned int depth)
	{
		// Pour éviter que la profondeur ne soit comptée dans le calcul des niveaux
		switch (type)
		{
			case ImageType_1D:
			case ImageType_1D_Array:
				return GetMaxLevel(width, 1U, 1U);

			case ImageType_2D:
			case ImageType_2D_Array:
			case ImageType_Cubemap:
				return GetMaxLevel(width, height, 1U);

			case ImageType_3D:
				return GetMaxLevel(width, height, depth);
		}

		NazaraError("Image type not handled (0x" + String::Number(type, 16) + ')');
		return 0;

	}

	void Image::EnsureOwnership()
	{
		if (m_sharedImage == &emptyImage)
			return;

		if (m_sharedImage->refCount > 1)
		{
			SharedImage::PixelContainer levels(m_sharedImage->levels.size());
			for (unsigned int i = 0; i < levels.size(); ++i)
			{
				unsigned int size = GetMemoryUsage(i);
				levels[i].reset(new UInt8[size]);
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

	bool Image::Initialize()
	{
		if (!ImageLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		if (!ImageManager::Initialize())
		{
			NazaraError("Failed to initialise manager");
			return false;
		}

		return true;
	}

	void Image::Uninitialize()
	{
		ImageManager::Uninitialize();
		ImageLibrary::Uninitialize();
	}

	Image::SharedImage Image::emptyImage(0, ImageType_2D, PixelFormatType_Undefined, Image::SharedImage::PixelContainer(), 0, 0, 0);
	ImageLibrary::LibraryMap Image::s_library;
	ImageLoader::LoaderList Image::s_loaders;
	ImageManager::ManagerMap Image::s_managerMap;
	ImageManager::ManagerParams Image::s_managerParameters;
	ImageSaver::SaverList Image::s_savers;
}
