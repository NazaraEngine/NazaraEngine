// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Image.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Config.hpp>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

///TODO: Rajouter des warnings (Formats compressés avec les méthodes Copy/Update, tests taille dans Copy)

namespace
{
	inline unsigned int GetLevelSize(unsigned int size, nzUInt8 level)
	{
		return std::max(size >> level, 1U);
	}

	inline nzUInt8* GetPixelPtr(nzUInt8* base, nzUInt8 bpp, unsigned int x, unsigned int y, unsigned int z, unsigned int width, unsigned int height)
	{
		return &base[(width*(height*z + y) + x)*bpp];
	}
}

bool NzImageParams::IsValid() const
{
	return true;
}

NzImage::NzImage() :
m_sharedImage(&emptyImage)
{
}

NzImage::NzImage(nzImageType type, nzPixelFormat format, unsigned int width, unsigned int height, unsigned int depth, nzUInt8 levelCount) :
m_sharedImage(&emptyImage)
{
	Create(type, format, width, height, depth, levelCount);

	#ifdef NAZARA_DEBUG
	if (!m_sharedImage)
	{
		NazaraError("Failed to create image");
		throw std::runtime_error("Constructor failed");
	}
	#endif
}

NzImage::NzImage(const NzImage& image) :
NzResource(image),
m_sharedImage(image.m_sharedImage)
{
	if (m_sharedImage != &emptyImage)
		m_sharedImage->refCount++;
}

NzImage::NzImage(NzImage&& image) noexcept :
m_sharedImage(image.m_sharedImage)
{
	image.m_sharedImage = &emptyImage;
}

NzImage::~NzImage()
{
	Destroy();
}

bool NzImage::Convert(nzPixelFormat format)
{
	#if NAZARA_UTILITY_SAFE
	if (m_sharedImage == &emptyImage)
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (!NzPixelFormat::IsValid(format))
	{
		NazaraError("Invalid pixel format");
		return false;
	}

	if (!NzPixelFormat::IsConversionSupported(m_sharedImage->format, format))
	{
		NazaraError("Conversion from " + NzPixelFormat::ToString(m_sharedImage->format) + " to " + NzPixelFormat::ToString(format) + " is not supported");
		return false;
	}
	#endif

	if (format == m_sharedImage->format)
		return true;

	nzUInt8** levels = new nzUInt8*[m_sharedImage->levelCount];

	unsigned int width = m_sharedImage->width;
	unsigned int height = m_sharedImage->height;

	// Les images 3D et cubemaps sont stockés de la même façon
	unsigned int depth = (m_sharedImage->type == nzImageType_Cubemap) ? 6 : m_sharedImage->depth;

	for (unsigned int i = 0; i < m_sharedImage->levelCount; ++i)
	{
		unsigned int pixelsPerFace = width*height;
		nzUInt8* face = new nzUInt8[pixelsPerFace*depth*NzPixelFormat::GetBytesPerPixel(format)];
		nzUInt8* ptr = face;
		nzUInt8* pixels = m_sharedImage->pixels[i];
		unsigned int srcStride = pixelsPerFace * NzPixelFormat::GetBytesPerPixel(m_sharedImage->format);
		unsigned int dstStride = pixelsPerFace * NzPixelFormat::GetBytesPerPixel(format);

		for (unsigned int d = 0; d < depth; ++d)
		{
			if (!NzPixelFormat::Convert(m_sharedImage->format, format, pixels, &pixels[srcStride], ptr))
			{
				NazaraError("Failed to convert image");

				// Nettoyage de la mémoire
				delete[] face; // Permet une optimisation de boucle (GCC)
				for (unsigned int j = 0; j < i; ++j)
					delete[] levels[j];

				delete[] levels;

				return false;
			}

			pixels += srcStride;
			ptr += dstStride;
		}

		levels[i] = face;

		if (width > 1)
			width >>= 1;

		if (height > 1)
			height >>= 1;

		if (depth > 1 && m_sharedImage->type != nzImageType_Cubemap)
			depth >>= 1;
	}

	SharedImage* newImage = new SharedImage(1, m_sharedImage->type, format, m_sharedImage->levelCount, levels, m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth);

	ReleaseImage();
	m_sharedImage = newImage;

	return true;
}

void NzImage::Copy(const NzImage& source, const NzCubeui& srcCube, const NzVector3ui& dstPos)
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

	const nzUInt8* srcPtr = source.GetConstPixels(srcCube.x, srcCube.y, srcCube.z);
	#if NAZARA_UTILITY_SAFE
	if (!srcPtr)
	{
		NazaraError("Failed to access pixels");
		return;
	}
	#endif

	nzUInt8 bpp = NzPixelFormat::GetBytesPerPixel(m_sharedImage->format);
	nzUInt8* dstPtr = GetPixelPtr(m_sharedImage->pixels[0], bpp, dstPos.x, dstPos.y, dstPos.z, m_sharedImage->width, m_sharedImage->height);

	Copy(dstPtr, srcPtr, bpp, srcCube.width, srcCube.height, srcCube.depth, m_sharedImage->width, m_sharedImage->height, source.GetWidth(), source.GetHeight());
}

bool NzImage::Create(nzImageType type, nzPixelFormat format, unsigned int width, unsigned int height, unsigned int depth, nzUInt8 levelCount)
{
	ReleaseImage();

	#if NAZARA_UTILITY_SAFE
	if (!NzPixelFormat::IsValid(format))
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
		case nzImageType_1D:
			if (height > 1)
			{
				NazaraError("1D textures must be 1 height");
				return false;
			}

			if (depth > 1)
			{
				NazaraError("1D textures must be 1 depth");
				return false;
			}
			break;

		case nzImageType_1D_Array:
		case nzImageType_2D:
			if (depth > 1)
			{
				NazaraError("2D textures must be 1 depth");
				return false;
			}
			break;

		case nzImageType_2D_Array:
		case nzImageType_3D:
			break;

		case nzImageType_Cubemap:
			if (depth > 1)
			{
				NazaraError("Cubemaps must be 1 depth");
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

	levelCount = std::min(levelCount, GetMaxLevel(width, height, depth));

	nzUInt8** levels = new nzUInt8*[levelCount];

	unsigned int w = width;
	unsigned int h = height;
	unsigned int d = (type == nzImageType_Cubemap) ? 6 : depth;

	for (unsigned int i = 0; i < levelCount; ++i)
	{
		// Cette allocation est protégée car sa taille dépend directement de paramètres utilisateurs
		try
		{
			levels[i] = new nzUInt8[w * h * d * NzPixelFormat::GetBytesPerPixel(format)];

			if (w > 1)
				w >>= 1;

			if (h > 1)
				h >>= 1;

			if (d > 1 && type != nzImageType_Cubemap)
				d >>= 1;
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to allocate image's level " + NzString::Number(i) + " (" + NzString(e.what()) + ')');

			// Nettoyage
			delete[] levels[i]; // Permet une optimisation de boucle (GCC)
			for (unsigned int j = 0; j < i; ++j)
				delete[] levels[j];

			delete[] levels;

			return false;
		}
	}

	m_sharedImage = new SharedImage(1, type, format, levelCount, levels, width, height, depth);

	NotifyCreated();
	return true;
}

void NzImage::Destroy()
{
	if (m_sharedImage != &emptyImage)
	{
		NotifyDestroy();
		ReleaseImage();
	}
}

bool NzImage::Fill(const NzColor& color)
{
	#if NAZARA_UTILITY_SAFE
	if (m_sharedImage == &emptyImage)
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (NzPixelFormat::IsCompressed(m_sharedImage->format))
	{
		NazaraError("Cannot access pixels from compressed image");
		return false;
	}
	#endif

	nzUInt8 bpp = NzPixelFormat::GetBytesPerPixel(m_sharedImage->format);
	std::unique_ptr<nzUInt8[]> colorBuffer(new nzUInt8[bpp]);
	if (!NzPixelFormat::Convert(nzPixelFormat_RGBA8, m_sharedImage->format, &color.r, colorBuffer.get()))
	{
		NazaraError("Failed to convert RGBA8 to " + NzPixelFormat::ToString(m_sharedImage->format));
		return false;
	}

	nzUInt8** levels = new nzUInt8*[m_sharedImage->levelCount];

	unsigned int width = m_sharedImage->width;
	unsigned int height = m_sharedImage->height;

	// Les images 3D et cubemaps sont stockés de la même façon
	unsigned int depth = (m_sharedImage->type == nzImageType_Cubemap) ? 6 : m_sharedImage->depth;

	for (unsigned int i = 0; i < m_sharedImage->levelCount; ++i)
	{
		unsigned int size = width*height*depth*bpp;
		nzUInt8* face = new nzUInt8[size];
		nzUInt8* ptr = face;
		nzUInt8* end = &ptr[size];

		while (ptr < end)
		{
			std::memcpy(ptr, colorBuffer.get(), bpp);
			ptr += bpp;
		}

		levels[i] = face;

		if (width > 1U)
			width >>= 1;

		if (height > 1U)
			height >>= 1;

		if (depth > 1U && m_sharedImage->type != nzImageType_Cubemap)
			depth >>= 1;
	}

	SharedImage* newImage = new SharedImage(1, m_sharedImage->type, m_sharedImage->format, m_sharedImage->levelCount, levels, m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth);

	ReleaseImage();
	m_sharedImage = newImage;

	return true;
}

bool NzImage::Fill(const NzColor& color, const NzRectui& rect, unsigned int z)
{
	#if NAZARA_UTILITY_SAFE
	if (m_sharedImage == &emptyImage)
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (!rect.IsValid())
	{
		NazaraError("Invalid rectangle");
		return false;
	}

	if (rect.x+rect.width > m_sharedImage->width || rect.y+rect.height > m_sharedImage->height)
	{
		NazaraError("Rectangle dimensions are out of bounds");
		return false;
	}

	unsigned int depth = (m_sharedImage->type == nzImageType_Cubemap) ? 6 : m_sharedImage->depth;
	if (z >= depth)
	{
		NazaraError("Z value exceeds depth (" + NzString::Number(z) + " >= (" + NzString::Number(depth) + ')');
		return false;
	}
	#endif

	EnsureOwnership();

	nzUInt8 bpp = NzPixelFormat::GetBytesPerPixel(m_sharedImage->format);
	std::unique_ptr<nzUInt8[]> colorBuffer(new nzUInt8[bpp]);
	if (!NzPixelFormat::Convert(nzPixelFormat_RGBA8, m_sharedImage->format, &color.r, colorBuffer.get()))
	{
		NazaraError("Failed to convert RGBA8 to " + NzPixelFormat::ToString(m_sharedImage->format));
		return false;
	}

	///FIXME: L'algorithme a du mal avec un bpp non multiple de 2
	nzUInt8* dstPixels = GetPixelPtr(m_sharedImage->pixels[0], bpp, rect.x, rect.y, z, m_sharedImage->width, m_sharedImage->height);
	unsigned int srcStride = rect.width * bpp;
	unsigned int dstStride = m_sharedImage->width * bpp;
	for (unsigned int y = 0; y < rect.height; ++y)
	{
		nzUInt8* start = dstPixels;
		nzUInt8* end = dstPixels + srcStride;
		while (start < end)
		{
			std::memcpy(start, colorBuffer.get(), bpp);
			start += bpp;
		}

		dstPixels += dstStride;
	}

	return true;
}

bool NzImage::Fill(const NzColor& color, const NzCubeui& cube)
{
	#if NAZARA_UTILITY_SAFE
	if (m_sharedImage == &emptyImage)
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (!cube.IsValid())
	{
		NazaraError("Invalid rectangle");
		return false;
	}

	if (cube.x+cube.width > m_sharedImage->width || cube.y+cube.height > m_sharedImage->height || cube.z+cube.depth > m_sharedImage->depth)
	{
		NazaraError("Cube dimensions are out of bounds");
		return false;
	}
	#endif

	EnsureOwnership();

	nzUInt8 bpp = NzPixelFormat::GetBytesPerPixel(m_sharedImage->format);
	std::unique_ptr<nzUInt8[]> colorBuffer(new nzUInt8[bpp]);
	if (!NzPixelFormat::Convert(nzPixelFormat_RGBA8, m_sharedImage->format, &color.r, colorBuffer.get()))
	{
		NazaraError("Failed to convert RGBA8 to " + NzPixelFormat::ToString(m_sharedImage->format));
		return false;
	}

	///FIXME: L'algorithme a du mal avec un bpp non multiple de 2
	nzUInt8* dstPixels = GetPixelPtr(m_sharedImage->pixels[0], bpp, cube.x, cube.y, cube.z, m_sharedImage->width, m_sharedImage->height);
	unsigned int srcStride = cube.width * bpp;
	unsigned int dstStride = m_sharedImage->width * bpp;
	unsigned int faceSize = dstStride * m_sharedImage->height;
	for (unsigned int z = 0; z < cube.depth; ++z)
	{
		nzUInt8* facePixels = dstPixels;
		for (unsigned int y = 0; y < cube.height; ++y)
		{
			nzUInt8* start = facePixels;
			nzUInt8* end = facePixels + srcStride;
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

bool NzImage::FlipHorizontally()
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
	unsigned int depth = (m_sharedImage->type == nzImageType_Cubemap) ? 6 : m_sharedImage->depth;
	for (unsigned int level = 0; level < m_sharedImage->levelCount; ++level)
	{
		if (!NzPixelFormat::Flip(nzPixelFlipping_Horizontally, m_sharedImage->format, width, height, depth, m_sharedImage->pixels[level], m_sharedImage->pixels[level]))
		{
			NazaraError("Failed to flip image");
			return false;
		}

		if (width > 1U)
			width >>= 1;

		if (height > 1U)
			height >>= 1;

		if (depth > 1U && m_sharedImage->type != nzImageType_Cubemap)
			depth >>= 1;
	}

	return true;
}

bool NzImage::FlipVertically()
{
	#if NAZARA_UTILITY_SAFE
	if (m_sharedImage == &emptyImage)
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (NzPixelFormat::IsCompressed(m_sharedImage->format))
	{
		NazaraError("Cannot flip compressed image");
		return false;
	}
	#endif

	EnsureOwnership();

	unsigned int width = m_sharedImage->width;
	unsigned int height = m_sharedImage->height;
	unsigned int depth = (m_sharedImage->type == nzImageType_Cubemap) ? 6 : m_sharedImage->depth;
	for (unsigned int level = 0; level < m_sharedImage->levelCount; ++level)
	{
		if (!NzPixelFormat::Flip(nzPixelFlipping_Vertically, m_sharedImage->format, width, height, depth, m_sharedImage->pixels[level], m_sharedImage->pixels[level]))
		{
			NazaraError("Failed to flip image");
			return false;
		}

		if (width > 1U)
			width >>= 1;

		if (height > 1U)
			height >>= 1;

		if (depth > 1U && m_sharedImage->type != nzImageType_Cubemap)
			depth >>= 1;
	}

	return true;
}

nzUInt8 NzImage::GetBytesPerPixel() const
{
	return NzPixelFormat::GetBytesPerPixel(m_sharedImage->format);
}

const nzUInt8* NzImage::GetConstPixels(unsigned int x, unsigned int y, unsigned int z, nzUInt8 level) const
{
	#if NAZARA_UTILITY_SAFE
	if (m_sharedImage == &emptyImage)
	{
		NazaraError("Image must be valid");
		return nullptr;
	}

	if (level >= m_sharedImage->levelCount)
	{
		NazaraError("Level out of bounds (" + NzString::Number(level) + " >= " + NzString::Number(m_sharedImage->levelCount) + ')');
		return nullptr;
	}
	#endif

	unsigned int width = GetLevelSize(m_sharedImage->width, level);
	#if NAZARA_UTILITY_SAFE
	if (x >= width)
	{
		NazaraError("X value exceeds width (" + NzString::Number(x) + " >= (" + NzString::Number(width) + ')');
		return nullptr;
	}
	#endif

	unsigned int height = GetLevelSize(m_sharedImage->height, level);
	#if NAZARA_UTILITY_SAFE
	if (y >= height)
	{
		NazaraError("Y value exceeds height (" + NzString::Number(y) + " >= (" + NzString::Number(height) + ')');
		return nullptr;
	}

	unsigned int depth = (m_sharedImage->type == nzImageType_Cubemap) ? 6 : GetLevelSize(m_sharedImage->depth, level);
	if (z >= depth)
	{
		NazaraError("Z value exceeds depth (" + NzString::Number(z) + " >= (" + NzString::Number(depth) + ')');
		return nullptr;
	}
	#endif

	return GetPixelPtr(m_sharedImage->pixels[level], NzPixelFormat::GetBytesPerPixel(m_sharedImage->format), x, y, z, width, height);
}

unsigned int NzImage::GetDepth(nzUInt8 level) const
{
	#if NAZARA_UTILITY_SAFE
	if (level >= m_sharedImage->levelCount)
	{
		NazaraError("Level out of bounds (" + NzString::Number(level) + " >= " + NzString::Number(m_sharedImage->levelCount) + ')');
		return 0;
	}
	#endif

	return GetLevelSize(m_sharedImage->depth, level);
}

nzPixelFormat NzImage::GetFormat() const
{
	return m_sharedImage->format;
}

unsigned int NzImage::GetHeight(nzUInt8 level) const
{
	#if NAZARA_UTILITY_SAFE
	if (level >= m_sharedImage->levelCount)
	{
		NazaraError("Level out of bounds (" + NzString::Number(level) + " >= " + NzString::Number(m_sharedImage->levelCount) + ')');
		return 0;
	}
	#endif

	return GetLevelSize(m_sharedImage->height, level);
}

nzUInt8 NzImage::GetLevelCount() const
{
	return m_sharedImage->levelCount;
}

nzUInt8 NzImage::GetMaxLevel() const
{
	return GetMaxLevel(m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth);
}

NzColor NzImage::GetPixelColor(unsigned int x, unsigned int y, unsigned int z) const
{
	#if NAZARA_UTILITY_SAFE
	if (m_sharedImage == &emptyImage)
	{
		NazaraError("Image must be valid");
		return NzColor();
	}

	if (NzPixelFormat::IsCompressed(m_sharedImage->format))
	{
		NazaraError("Cannot access pixels from compressed image");
		return NzColor();
	}

	if (x >= m_sharedImage->width)
	{
		NazaraError("X value exceeds width (" + NzString::Number(x) + " >= (" + NzString::Number(m_sharedImage->width) + ')');
		return NzColor();
	}

	if (y >= m_sharedImage->height)
	{
		NazaraError("Y value exceeds height (" + NzString::Number(y) + " >= (" + NzString::Number(m_sharedImage->height) + ')');
		return NzColor();
	}

	unsigned int depth = (m_sharedImage->type == nzImageType_Cubemap) ? 6 : m_sharedImage->depth;
	if (z >= depth)
	{
		NazaraError("Z value exceeds depth (" + NzString::Number(z) + " >= (" + NzString::Number(depth) + ')');
		return NzColor();
	}
	#endif

	const nzUInt8* pixel = GetPixelPtr(m_sharedImage->pixels[0], NzPixelFormat::GetBytesPerPixel(m_sharedImage->format), x, y, z, m_sharedImage->width, m_sharedImage->height);

	NzColor color;
	if (!NzPixelFormat::Convert(m_sharedImage->format, nzPixelFormat_RGBA8, pixel, &color.r))
		NazaraError("Failed to convert image's format to RGBA8");

	return color;
}

nzUInt8* NzImage::GetPixels(unsigned int x, unsigned int y, unsigned int z, nzUInt8 level)
{
	#if NAZARA_UTILITY_SAFE
	if (m_sharedImage == &emptyImage)
	{
		NazaraError("Image must be valid");
		return nullptr;
	}

	if (level >= m_sharedImage->levelCount)
	{
		NazaraError("Level out of bounds (" + NzString::Number(level) + " >= " + NzString::Number(m_sharedImage->levelCount) + ')');
		return nullptr;
	}
	#endif

	unsigned int width = GetLevelSize(m_sharedImage->width, level);
	#if NAZARA_UTILITY_SAFE
	if (x >= width)
	{
		NazaraError("X value exceeds width (" + NzString::Number(x) + " >= (" + NzString::Number(width) + ')');
		return nullptr;
	}
	#endif

	unsigned int height = GetLevelSize(m_sharedImage->height, level);
	#if NAZARA_UTILITY_SAFE
	if (y >= height)
	{
		NazaraError("Y value exceeds height (" + NzString::Number(y) + " >= (" + NzString::Number(height) + ')');
		return nullptr;
	}

	unsigned int depth = (m_sharedImage->type == nzImageType_Cubemap) ? 6 : GetLevelSize(m_sharedImage->depth, level);
	if (z >= depth)
	{
		NazaraError("Z value exceeds depth (" + NzString::Number(z) + " >= (" + NzString::Number(depth) + ')');
		return nullptr;
	}

	if (level >= m_sharedImage->levelCount)
	{
		NazaraError("Level out of bounds (" + NzString::Number(level) + " >= " + NzString::Number(m_sharedImage->levelCount) + ')');
		return nullptr;
	}
	#endif

	EnsureOwnership();

	return GetPixelPtr(m_sharedImage->pixels[level], NzPixelFormat::GetBytesPerPixel(m_sharedImage->format), x, y, z, m_sharedImage->width, m_sharedImage->height);
}

unsigned int NzImage::GetSize() const
{
	unsigned int width = m_sharedImage->width;
	unsigned int height = m_sharedImage->height;
	unsigned int depth = m_sharedImage->depth;

	unsigned int size = 0;
	for (unsigned int i = 0; i < m_sharedImage->levelCount; ++i)
	{
		size += width * height * depth;

		if (width > 1)
			width >>= 1;

		if (height > 1)
			height >>= 1;

		if (depth > 1)
			depth >>= 1;
	}

	if (m_sharedImage->type == nzImageType_Cubemap)
		size *= 6;

	return size * NzPixelFormat::GetBytesPerPixel(m_sharedImage->format);
}

unsigned int NzImage::GetSize(nzUInt8 level) const
{
	#if NAZARA_UTILITY_SAFE
	if (level >= m_sharedImage->levelCount)
	{
		NazaraError("Level out of bounds (" + NzString::Number(level) + " >= " + NzString::Number(m_sharedImage->levelCount) + ')');
		return 0;
	}
	#endif

	return (GetLevelSize(m_sharedImage->width, level)) *
	       (GetLevelSize(m_sharedImage->height, level)) *
	       ((m_sharedImage->type == nzImageType_Cubemap) ? 6 : GetLevelSize(m_sharedImage->depth, level)) *
	       NzPixelFormat::GetBytesPerPixel(m_sharedImage->format);
}

nzImageType	NzImage::GetType() const
{
	return m_sharedImage->type;
}

unsigned int NzImage::GetWidth(nzUInt8 level) const
{
	#if NAZARA_UTILITY_SAFE
	if (level >= m_sharedImage->levelCount)
	{
		NazaraError("Level out of bounds (" + NzString::Number(level) + " >= " + NzString::Number(m_sharedImage->levelCount) + ')');
		return 0;
	}
	#endif

	return GetLevelSize(m_sharedImage->width, level);
}

bool NzImage::IsCompressed() const
{
	return NzPixelFormat::IsCompressed(m_sharedImage->format);
}

bool NzImage::IsCubemap() const
{
	return m_sharedImage->type == nzImageType_Cubemap;
}

bool NzImage::IsValid() const
{
	return m_sharedImage != &emptyImage;
}

bool NzImage::LoadFromFile(const NzString& filePath, const NzImageParams& params)
{
	return NzImageLoader::LoadFromFile(this, filePath, params);
}

bool NzImage::LoadFromMemory(const void* data, std::size_t size, const NzImageParams& params)
{
	return NzImageLoader::LoadFromMemory(this, data, size, params);
}

bool NzImage::LoadFromStream(NzInputStream& stream, const NzImageParams& params)
{
	return NzImageLoader::LoadFromStream(this, stream, params);
}

bool NzImage::LoadCubemapFromFile(const NzString& filePath, const NzImageParams& imageParams, const NzCubemapParams& cubemapParams)
{
	NzImage image;
	if (!image.LoadFromFile(filePath, imageParams))
	{
		NazaraError("Failed to load image");
		return false;
	}

	return LoadCubemapFromImage(image, cubemapParams);
}

bool NzImage::LoadCubemapFromImage(const NzImage& image, const NzCubemapParams& params)
{
	#if NAZARA_UTILITY_SAFE
	if (!image.IsValid())
	{
		NazaraError("Image must be valid");
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

	NzVector2ui backPos = params.backPosition * faceSize;
	if (backPos.x > limitX || backPos.y > limitY)
	{
		NazaraError("Back rectangle is out of image");
		return false;
	}

	NzVector2ui downPos = params.downPosition * faceSize;
	if (downPos.x > limitX || downPos.y > limitY)
	{
		NazaraError("Down rectangle is out of image");
		return false;
	}

	NzVector2ui forwardPos = params.forwardPosition * faceSize;
	if (forwardPos.x > limitX || forwardPos.y > limitY)
	{
		NazaraError("Forward rectangle is out of image");
		return false;
	}

	NzVector2ui leftPos = params.leftPosition * faceSize;
	if (leftPos.x > limitX || leftPos.y > limitY)
	{
		NazaraError("Left rectangle is out of image");
		return false;
	}

	NzVector2ui rightPos = params.rightPosition * faceSize;
	if (rightPos.x > limitX || rightPos.y > limitY)
	{
		NazaraError("Right rectangle is out of image");
		return false;
	}

	NzVector2ui upPos = params.upPosition * faceSize;
	if (upPos.x > limitX || upPos.y > limitY)
	{
		NazaraError("Up rectangle is out of image");
		return false;
	}

	Create(nzImageType_Cubemap, image.GetFormat(), faceSize, faceSize);

	#ifdef NAZARA_DEBUG
	// Les paramètres sont valides, que Create ne fonctionne pas relèverait d'un bug
	if (m_sharedImage == &emptyImage)
	{
		NazaraInternalError("Failed to create cubemap");
		return false;
	}
	#endif

	Copy(image, NzRectui(backPos.x, backPos.y, faceSize, faceSize), NzVector3ui(0, 0, nzCubemapFace_NegativeZ));
	Copy(image, NzRectui(downPos.x, downPos.y, faceSize, faceSize), NzVector3ui(0, 0, nzCubemapFace_NegativeY));
	Copy(image, NzRectui(forwardPos.x, forwardPos.y, faceSize, faceSize), NzVector3ui(0, 0, nzCubemapFace_PositiveZ));
	Copy(image, NzRectui(leftPos.x, leftPos.y, faceSize, faceSize), NzVector3ui(0, 0, nzCubemapFace_NegativeX));
	Copy(image, NzRectui(rightPos.x, rightPos.y, faceSize, faceSize), NzVector3ui(0, 0, nzCubemapFace_PositiveX));
	Copy(image, NzRectui(upPos.x, upPos.y, faceSize, faceSize), NzVector3ui(0, 0, nzCubemapFace_PositiveY));

	return true;
}

bool NzImage::LoadCubemapFromMemory(const void* data, std::size_t size, const NzImageParams& imageParams, const NzCubemapParams& cubemapParams)
{
	NzImage image;
	if (!image.LoadFromMemory(data, size, imageParams))
	{
		NazaraError("Failed to load image");
		return false;
	}

	return LoadCubemapFromImage(image, cubemapParams);
}

bool NzImage::LoadCubemapFromStream(NzInputStream& stream, const NzImageParams& imageParams, const NzCubemapParams& cubemapParams)
{
	NzImage image;
	if (!image.LoadFromStream(stream, imageParams))
	{
		NazaraError("Failed to load image");
		return false;
	}

	return LoadCubemapFromImage(image, cubemapParams);
}

void NzImage::SetLevelCount(nzUInt8 levelCount)
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

	levelCount = std::min(levelCount, GetMaxLevel(m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth));

	if (m_sharedImage->levelCount == levelCount)
		return;

	EnsureOwnership();

	nzUInt8 oldLevelCount = m_sharedImage->levelCount;
	nzUInt8 maxLevelCount = std::max(levelCount, oldLevelCount);
	m_sharedImage->levelCount = levelCount; // Pour faire fonctionner GetSize

	nzUInt8** pixels = new nzUInt8*[levelCount];
	for (unsigned int i = 0; i < maxLevelCount; ++i)
	{
		if (i < oldLevelCount)
			pixels[i] = m_sharedImage->pixels[i];
		else if (i < levelCount)
			pixels[i] = new nzUInt8[GetSize(i)];
		else
			delete[] m_sharedImage->pixels[i];
	}

	delete[] m_sharedImage->pixels;

	m_sharedImage->pixels = pixels;
}

bool NzImage::SetPixelColor(const NzColor& color, unsigned int x, unsigned int y, unsigned int z)
{
	#if NAZARA_UTILITY_SAFE
	if (m_sharedImage == &emptyImage)
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (NzPixelFormat::IsCompressed(m_sharedImage->format))
	{
		NazaraError("Cannot access pixels from compressed image");
		return false;
	}

	if (x >= m_sharedImage->width)
	{
		NazaraError("X value exceeds width (" + NzString::Number(x) + " >= (" + NzString::Number(m_sharedImage->width) + ')');
		return false;
	}

	if (y >= m_sharedImage->height)
	{
		NazaraError("Y value exceeds height (" + NzString::Number(y) + " >= (" + NzString::Number(m_sharedImage->height) + ')');
		return false;
	}

	unsigned int depth = (m_sharedImage->type == nzImageType_Cubemap) ? 6 : m_sharedImage->depth;
	if (z >= depth)
	{
		NazaraError("Z value exceeds depth (" + NzString::Number(z) + " >= (" + NzString::Number(depth) + ')');
		return false;
	}
	#endif

	nzUInt8* pixel = GetPixelPtr(m_sharedImage->pixels[0], NzPixelFormat::GetBytesPerPixel(m_sharedImage->format), x, y, z, m_sharedImage->width, m_sharedImage->height);

	if (!NzPixelFormat::Convert(nzPixelFormat_RGBA8, m_sharedImage->format, &color.r, pixel))
	{
		NazaraError("Failed to convert RGBA8 to image's format");
		return false;
	}

	return true;
}

void NzImage::Update(const nzUInt8* pixels, unsigned int srcWidth, unsigned int srcHeight, nzUInt8 level)
{
	#if NAZARA_UTILITY_SAFE
	if (m_sharedImage == &emptyImage)
	{
		NazaraError("Image must be valid");
		return;
	}

	if (!pixels)
	{
		NazaraError("Invalid pixel source");
		return;
	}

	if (level >= m_sharedImage->levelCount)
	{
		NazaraError("Level out of bounds (" + NzString::Number(level) + " >= " + NzString::Number(m_sharedImage->levelCount) + ')');
		return;
	}
	#endif

	EnsureOwnership();

	Copy(m_sharedImage->pixels[level], pixels, NzPixelFormat::GetBytesPerPixel(m_sharedImage->format),
	     GetLevelSize(m_sharedImage->width, level),
	     GetLevelSize(m_sharedImage->height, level),
	     GetLevelSize(m_sharedImage->depth, level),
	     0, 0,
	     srcWidth, srcHeight);
}

void NzImage::Update(const nzUInt8* pixels, const NzRectui& rect, unsigned int z, unsigned int srcWidth, unsigned int srcHeight, nzUInt8 level)
{
	#if NAZARA_UTILITY_SAFE
	if (m_sharedImage == &emptyImage)
	{
		NazaraError("Image must be valid");
		return;
	}

	if (!pixels)
	{
		NazaraError("Invalid pixel source");
		return;
	}

	if (!rect.IsValid())
	{
		NazaraError("Invalid rectangle");
		return;
	}

	if (level >= m_sharedImage->levelCount)
	{
		NazaraError("Level out of bounds (" + NzString::Number(level) + " >= " + NzString::Number(m_sharedImage->levelCount) + ')');
		return;
	}
	#endif

	unsigned int width = GetLevelSize(m_sharedImage->width, level);
	unsigned int height = GetLevelSize(m_sharedImage->height, level);

	#if NAZARA_UTILITY_SAFE
	if (rect.x+rect.width > width || rect.y+rect.height > height)
	{
		NazaraError("Rectangle dimensions are out of bounds");
		return;
	}

	unsigned int depth = (m_sharedImage->type == nzImageType_Cubemap) ? 6 : GetLevelSize(m_sharedImage->depth, level);
	if (z >= depth)
	{
		NazaraError("Z value exceeds depth (" + NzString::Number(z) + " >= " + NzString::Number(depth) + ')');
		return;
	}
	#endif

	EnsureOwnership();

	nzUInt8 bpp = NzPixelFormat::GetBytesPerPixel(m_sharedImage->format);
	nzUInt8* dstPixels = GetPixelPtr(m_sharedImage->pixels[level], bpp, rect.x, rect.y, z, width, height);

	Copy(dstPixels, pixels, bpp,
	     rect.width, rect.height, 1,
	     width, height,
	     srcWidth, srcHeight);
}

void NzImage::Update(const nzUInt8* pixels, const NzCubeui& cube, unsigned int srcWidth, unsigned int srcHeight, nzUInt8 level)
{
	#if NAZARA_UTILITY_SAFE
	if (m_sharedImage == &emptyImage)
	{
		NazaraError("Image must be valid");
		return;
	}

	if (!pixels)
	{
		NazaraError("Invalid pixel source");
		return;
	}

	if (level >= m_sharedImage->levelCount)
	{
		NazaraError("Level out of bounds (" + NzString::Number(level) + " >= " + NzString::Number(m_sharedImage->levelCount) + ')');
		return;
	}
	#endif

	unsigned int width = GetLevelSize(m_sharedImage->width, level);
	unsigned int height = GetLevelSize(m_sharedImage->height, level);

	#if NAZARA_UTILITY_SAFE
	if (!cube.IsValid())
	{
		NazaraError("Invalid cube");
		return;
	}

	// Nous n'autorisons pas de modifier plus d'une face du cubemap à la fois
	if (cube.x+cube.width > width || cube.y+cube.height > height || cube.z+cube.depth > GetLevelSize(m_sharedImage->height, level))
	{
		NazaraError("Cube dimensions are out of bounds");
		return;
	}
	#endif

	EnsureOwnership();

	nzUInt8 bpp = NzPixelFormat::GetBytesPerPixel(m_sharedImage->format);
	nzUInt8* dstPixels = GetPixelPtr(m_sharedImage->pixels[level], bpp, cube.x, cube.y, cube.z, width, height);

	Copy(dstPixels, pixels, bpp,
	     cube.width, cube.height, cube.depth,
	     width, height,
	     srcWidth, srcHeight);
}

NzImage& NzImage::operator=(const NzImage& image)
{
	ReleaseImage();

	m_sharedImage = image.m_sharedImage;
	if (m_sharedImage != &emptyImage)
		m_sharedImage->refCount++;

	return *this;
}

NzImage& NzImage::operator=(NzImage&& image) noexcept
{
	std::swap(m_sharedImage, image.m_sharedImage);

	return *this;
}

void NzImage::Copy(nzUInt8* destination, const nzUInt8* source, nzUInt8 bpp, unsigned int width, unsigned int height, unsigned int depth, unsigned int dstWidth, unsigned int dstHeight, unsigned int srcWidth, unsigned int srcHeight)
{
	if (dstWidth == 0)
		dstWidth = width;

	if (dstHeight == 0)
		dstHeight = height;

	if (srcWidth == 0)
		srcWidth = width;

	if (srcHeight == 0)
		srcHeight = height;

	if ((height == 1 || (dstWidth == width && srcWidth == width)) && (depth == 1 || (dstHeight == height && srcHeight == height)))
		std::memcpy(destination, source, width*height*depth*bpp);
	else
	{
		unsigned int lineStride = width * bpp;
		unsigned int dstLineStride = dstWidth * bpp;
		unsigned int dstFaceStride = dstLineStride * dstHeight;
		unsigned int srcLineStride = srcWidth * bpp;
		unsigned int srcFaceStride = srcLineStride * srcHeight;

		for (unsigned int i = 0; i < depth; ++i)
		{
			nzUInt8* dstFacePtr = destination;
			const nzUInt8* srcFacePtr = source;
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

nzUInt8 NzImage::GetMaxLevel(unsigned int width, unsigned int height, unsigned int depth)
{
	static const float invLog2 = 1.f/std::log(2.f);

	unsigned int widthLevel = invLog2 * std::log(static_cast<float>(width));
	unsigned int heightLevel = invLog2 * std::log(static_cast<float>(height));
	unsigned int depthLevel = invLog2 * std::log(static_cast<float>(depth));

	return std::max(std::max(std::max(widthLevel, heightLevel), depthLevel), 1U);
}

void NzImage::EnsureOwnership()
{
	if (m_sharedImage == &emptyImage)
		return;

	if (m_sharedImage->refCount > 1)
	{
		m_sharedImage->refCount--;

		nzUInt8** pixels = new nzUInt8*[m_sharedImage->levelCount];
		for (unsigned int i = 0; i < m_sharedImage->levelCount; ++i)
		{
			unsigned int size = GetSize(i);
			pixels[i] = new nzUInt8[size];
			std::memcpy(pixels[i], m_sharedImage->pixels[i], size);
		}

		m_sharedImage = new SharedImage(1, m_sharedImage->type, m_sharedImage->format, m_sharedImage->levelCount, pixels, m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth);
	}
}

void NzImage::ReleaseImage()
{
	if (m_sharedImage == &emptyImage)
		return;

	if (--m_sharedImage->refCount == 0)
	{
		for (unsigned int i = 0; i < m_sharedImage->levelCount; ++i)
			delete[] m_sharedImage->pixels[i];

		delete[] m_sharedImage->pixels;
		delete m_sharedImage;
	}

	m_sharedImage = &emptyImage;
}

NzImage::SharedImage NzImage::emptyImage(0, nzImageType_2D, nzPixelFormat_Undefined, 1, nullptr, 0, 0, 0);
NzImageLoader::LoaderList NzImage::s_loaders;
