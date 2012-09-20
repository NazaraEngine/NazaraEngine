// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Image.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Config.hpp>
#include <cmath>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

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
	{
		NazaraMutexLock(m_sharedImage->mutex);
		m_sharedImage->refCount++;
		NazaraMutexUnlock(m_sharedImage->mutex);
	}
}

NzImage::NzImage(NzImage&& image) noexcept :
m_sharedImage(image.m_sharedImage)
{
	image.m_sharedImage = &emptyImage;
}

NzImage::~NzImage()
{
	ReleaseImage();
}

bool NzImage::Convert(nzPixelFormat format)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImage)
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
		nzUInt8* ptr = new nzUInt8[pixelsPerFace*depth*NzPixelFormat::GetBPP(format)];
		nzUInt8* pixels = m_sharedImage->pixels[i];
		unsigned int srcStride = pixelsPerFace * NzPixelFormat::GetBPP(m_sharedImage->format);
		unsigned int dstStride = pixelsPerFace * NzPixelFormat::GetBPP(format);

		for (unsigned int d = 0; d < depth; ++d)
		{
			if (!NzPixelFormat::Convert(m_sharedImage->format, format, pixels, &pixels[srcStride], ptr))
			{
				NazaraError("Failed to convert image");

				// Nettoyage de la mémoire
				delete[] ptr; // Permet une optimisation de boucle (GCC)
				for (unsigned int j = 0; j < i; ++j)
					delete[] levels[j];

				delete[] levels;

				return false;
			}

			pixels += srcStride;
			ptr += dstStride;
		}

		levels[i] = ptr;

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

bool NzImage::Copy(const NzImage& source, const NzCubeui& srcCube, const NzVector3ui& dstPos)
{
	#if NAZARA_UTILITY_SAFE
	if (!source.IsValid())
	{
		NazaraError("Source image must be valid");
		return false;
	}

	if (source.GetFormat() != m_sharedImage->format)
	{
		NazaraError("Source image format does not match destination image format");
		return false;
	}
	#endif

	const nzUInt8* pixels = source.GetConstPixels(0, srcCube.x, srcCube.y, srcCube.z);
	if (!pixels)
	{
		NazaraError("Failed to access pixels");
		return false;
	}

	/*
	Correctif temporaire : Update veut de la mémoire contigüe
	Il est donc nécessaire de prendre la partie de la texture que nous voulons mettre à jour

	///FIXME: Trouver une interface pour gérer ce genre de problème (Façon OpenGL?)
	(Appliquer l'interface à NzTexture également)
	*/
	nzUInt8 bpp = NzPixelFormat::GetBPP(m_sharedImage->format);
	unsigned int dstLineStride = srcCube.width*bpp;
	unsigned int dstFaceStride = dstLineStride*srcCube.height;
	unsigned int srcLineStride = m_sharedImage->width*bpp;
	unsigned int srcFaceStride = srcLineStride*m_sharedImage->height;

	nzUInt8* cube = new nzUInt8[dstFaceStride*srcCube.depth];
	nzUInt8* ptr = cube;

	for (unsigned int z = 0; z < srcCube.depth; ++z)
	{
		nzUInt8* facePixels = ptr;
		for (unsigned int y = 0; y < srcCube.height; ++y)
		{
			std::memcpy(facePixels, pixels, dstLineStride);

			facePixels += dstLineStride;
			pixels += srcLineStride;
		}

		ptr += dstFaceStride;
		pixels += srcFaceStride;
	}

	bool success = Update(cube, NzCubeui(dstPos.x, dstPos.y, dstPos.z, srcCube.width, srcCube.height, srcCube.depth));

	delete[] cube;

	return success;
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
			levels[i] = new nzUInt8[w * h * d * NzPixelFormat::GetBPP(format)];

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
	NotifyDestroy();
	ReleaseImage();
}

bool NzImage::Fill(const NzColor& color)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImage)
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

	EnsureOwnership();

	nzUInt8 bpp = NzPixelFormat::GetBPP(m_sharedImage->format);
	nzUInt8* pixels = new nzUInt8[bpp];
	if (!NzPixelFormat::Convert(nzPixelFormat_RGBA8, m_sharedImage->format, &color.r, pixels))
	{
		NazaraError("Failed to convert RGBA8 to " + NzPixelFormat::ToString(m_sharedImage->format));
		delete[] pixels;

		return false;
	}

	unsigned int width = m_sharedImage->width;
	unsigned int height = m_sharedImage->height;
	unsigned int depth = (m_sharedImage->type == nzImageType_Cubemap) ? 6 : m_sharedImage->depth;

	for (unsigned int level = 0; level < m_sharedImage->levelCount; ++level)
	{
		nzUInt8* ptr = &m_sharedImage->pixels[level][0];
		nzUInt8* end = &m_sharedImage->pixels[level][width*height*depth*bpp];

		while (ptr < end)
		{
			std::memcpy(ptr, pixels, bpp);
			ptr += bpp;
		}

		if (width > 1U)
			width >>= 1;

		if (height > 1U)
			height >>= 1;

		if (depth > 1U && m_sharedImage->type != nzImageType_Cubemap)
			depth >>= 1;
	}

	delete[] pixels;

	return true;
}

bool NzImage::Fill(const NzColor& color, const NzRectui& rect, unsigned int z)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImage)
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

	nzUInt8 bpp = NzPixelFormat::GetBPP(m_sharedImage->format);
	nzUInt8* pixels = new nzUInt8[bpp];
	if (!NzPixelFormat::Convert(nzPixelFormat_RGBA8, m_sharedImage->format, &color.r, pixels))
	{
		NazaraError("Failed to convert RGBA8 to " + NzPixelFormat::ToString(m_sharedImage->format));
		delete[] pixels;

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
			std::memcpy(start, pixels, bpp);
			start += bpp;
		}

		dstPixels += dstStride;
	}

	delete[] pixels;

	return true;
}

bool NzImage::Fill(const NzColor& color, const NzCubeui& cube)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImage)
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

	nzUInt8 bpp = NzPixelFormat::GetBPP(m_sharedImage->format);
	nzUInt8* pixels = new nzUInt8[bpp];
	if (!NzPixelFormat::Convert(nzPixelFormat_RGBA8, m_sharedImage->format, &color.r, pixels))
	{
		NazaraError("Failed to convert RGBA8 to " + NzPixelFormat::ToString(m_sharedImage->format));
		delete[] pixels;

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
				std::memcpy(start, pixels, bpp);
				start += bpp;
			}

			facePixels += dstStride;
		}

		dstPixels += faceSize;
	}

	delete[] pixels;

	return true;
}

bool NzImage::FlipHorizontally()
{
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImage)
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
		NzPixelFormat::Flip(nzPixelFlipping_Horizontally, m_sharedImage->format, width, height, depth, m_sharedImage->pixels[level], m_sharedImage->pixels[level]);

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
	if (!m_sharedImage)
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
		NzPixelFormat::Flip(nzPixelFlipping_Vertically, m_sharedImage->format, width, height, depth, m_sharedImage->pixels[level], m_sharedImage->pixels[level]);

		if (width > 1U)
			width >>= 1;

		if (height > 1U)
			height >>= 1;

		if (depth > 1U && m_sharedImage->type != nzImageType_Cubemap)
			depth >>= 1;
	}

	return true;
}

nzUInt8 NzImage::GetBPP() const
{
	return NzPixelFormat::GetBPP(m_sharedImage->format);
}

const nzUInt8* NzImage::GetConstPixels(unsigned int x, unsigned int y, unsigned int z, nzUInt8 level) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImage)
	{
		NazaraError("Image must be valid");
		return nullptr;
	}

	if (x >= m_sharedImage->width)
	{
		NazaraError("X value exceeds width (" + NzString::Number(x) + " >= (" + NzString::Number(m_sharedImage->width) + ')');
		return nullptr;
	}

	if (y >= m_sharedImage->height)
	{
		NazaraError("Y value exceeds width (" + NzString::Number(y) + " >= (" + NzString::Number(m_sharedImage->height) + ')');
		return nullptr;
	}

	unsigned int depth = (m_sharedImage->type == nzImageType_Cubemap) ? 6 : m_sharedImage->depth;
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

	return GetPixelPtr(m_sharedImage->pixels[level], NzPixelFormat::GetBPP(m_sharedImage->format), x, y, z, m_sharedImage->width, m_sharedImage->height);
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
	if (!m_sharedImage)
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
		NazaraError("Y value exceeds width (" + NzString::Number(y) + " >= (" + NzString::Number(m_sharedImage->height) + ')');
		return NzColor();
	}

	unsigned int depth = (m_sharedImage->type == nzImageType_Cubemap) ? 6 : m_sharedImage->depth;
	if (z >= depth)
	{
		NazaraError("Z value exceeds depth (" + NzString::Number(z) + " >= (" + NzString::Number(depth) + ')');
		return NzColor();
	}
	#endif

	const nzUInt8* pixel = GetPixelPtr(m_sharedImage->pixels[0], NzPixelFormat::GetBPP(m_sharedImage->format), x, y, z, m_sharedImage->width, m_sharedImage->height);

	NzColor color;
	if (!NzPixelFormat::Convert(m_sharedImage->format, nzPixelFormat_RGBA8, pixel, &color.r))
		NazaraError("Failed to convert image's format to RGBA8");

	return color;
}

nzUInt8* NzImage::GetPixels(unsigned int x, unsigned int y, unsigned int z, nzUInt8 level)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImage)
	{
		NazaraError("Image must be valid");
		return nullptr;
	}

	if (x >= m_sharedImage->width)
	{
		NazaraError("X value exceeds width (" + NzString::Number(x) + " >= (" + NzString::Number(m_sharedImage->width) + ')');
		return nullptr;
	}

	if (y >= m_sharedImage->height)
	{
		NazaraError("Y value exceeds width (" + NzString::Number(y) + " >= (" + NzString::Number(m_sharedImage->height) + ')');
		return nullptr;
	}

	unsigned int depth = (m_sharedImage->type == nzImageType_Cubemap) ? 6 : m_sharedImage->depth;
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

	return GetPixelPtr(m_sharedImage->pixels[level], NzPixelFormat::GetBPP(m_sharedImage->format), x, y, z, m_sharedImage->width, m_sharedImage->height);
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

	return size * NzPixelFormat::GetBPP(m_sharedImage->format);
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
	       NzPixelFormat::GetBPP(m_sharedImage->format);
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

bool NzImage::SetLevelCount(nzUInt8 levelCount)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImage)
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (levelCount == 0)
	{
		NazaraError("Level count must be positive");
		return false;
	}
	#endif

	levelCount = std::min(levelCount, GetMaxLevel(m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth));

	if (m_sharedImage->levelCount == levelCount)
		return true;

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

	return true;
}

bool NzImage::SetPixelColor(const NzColor& color, unsigned int x, unsigned int y, unsigned int z)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImage)
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
		NazaraError("Y value exceeds width (" + NzString::Number(y) + " >= (" + NzString::Number(m_sharedImage->height) + ')');
		return false;
	}

	unsigned int depth = (m_sharedImage->type == nzImageType_Cubemap) ? 6 : m_sharedImage->depth;
	if (z >= depth)
	{
		NazaraError("Z value exceeds depth (" + NzString::Number(z) + " >= (" + NzString::Number(depth) + ')');
		return false;
	}
	#endif

	nzUInt8* pixel = GetPixelPtr(m_sharedImage->pixels[0], NzPixelFormat::GetBPP(m_sharedImage->format), x, y, z, m_sharedImage->width, m_sharedImage->height);

	if (!NzPixelFormat::Convert(nzPixelFormat_RGBA8, m_sharedImage->format, &color.r, pixel))
	{
		NazaraError("Failed to convert RGBA8 to image's format");
		return false;
	}

	return true;
}

bool NzImage::Update(const nzUInt8* pixels, nzUInt8 level)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImage)
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (!pixels)
	{
		NazaraError("Invalid pixel source");
		return false;
	}

	if (level >= m_sharedImage->levelCount)
	{
		NazaraError("Level out of bounds (" + NzString::Number(level) + " >= " + NzString::Number(m_sharedImage->levelCount) + ')');
		return false;
	}
	#endif

	EnsureOwnership();

	std::memcpy(m_sharedImage->pixels[level], pixels, GetSize(level));

	return true;
}

bool NzImage::Update(const nzUInt8* pixels, const NzRectui& rect, unsigned int z, nzUInt8 level)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImage)
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (!pixels)
	{
		NazaraError("Invalid pixel source");
		return false;
	}

	if (level >= m_sharedImage->levelCount)
	{
		NazaraError("Level out of bounds (" + NzString::Number(level) + " >= " + NzString::Number(m_sharedImage->levelCount) + ')');
		return false;
	}

	if (!rect.IsValid())
	{
		NazaraError("Invalid rectangle");
		return false;
	}
	#endif

	unsigned int width = GetLevelSize(m_sharedImage->width, level);
	unsigned int height = GetLevelSize(m_sharedImage->height, level);

	#if NAZARA_UTILITY_SAFE
	if (rect.x+rect.width > width || rect.y+rect.height > height)
	{
		NazaraError("Rectangle dimensions are out of bounds");
		return false;
	}

	unsigned int depth = (m_sharedImage->type == nzImageType_Cubemap) ? 6 : GetLevelSize(m_sharedImage->depth, level);
	if (z >= depth)
	{
		NazaraError("Z value exceeds depth (" + NzString::Number(z) + " >= " + NzString::Number(depth) + ')');
		return false;
	}
	#endif

	EnsureOwnership();

	nzUInt8 bpp = NzPixelFormat::GetBPP(m_sharedImage->format);
	nzUInt8* dstPixels = GetPixelPtr(m_sharedImage->pixels[level], bpp, rect.x, rect.y, z, width, height);
	unsigned int srcStride = rect.width * bpp;
	unsigned int dstStride = m_sharedImage->width * bpp;
	for (unsigned int y = 0; y < rect.height; ++y)
	{
		std::memcpy(dstPixels, pixels, srcStride);
		pixels += srcStride;
		dstPixels += dstStride;
	}

	return true;
}

bool NzImage::Update(const nzUInt8* pixels, const NzCubeui& cube, nzUInt8 level)
{
	///FIXME: Vérifier que ça fonctionne correctement
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImage)
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (!pixels)
	{
		NazaraError("Invalid pixel source");
		return false;
	}

	if (level >= m_sharedImage->levelCount)
	{
		NazaraError("Level out of bounds (" + NzString::Number(level) + " >= " + NzString::Number(m_sharedImage->levelCount) + ')');
		return false;
	}
	#endif

	unsigned int width = GetLevelSize(m_sharedImage->width, level);
	unsigned int height = GetLevelSize(m_sharedImage->height, level);
	unsigned int depth = (m_sharedImage->type == nzImageType_Cubemap) ? 6 : GetLevelSize(m_sharedImage->height, level);

	#if NAZARA_UTILITY_SAFE
	if (!cube.IsValid())
	{
		NazaraError("Invalid cube");
		return false;
	}

	if (cube.x+cube.width > width || cube.y+cube.height > height || cube.z+cube.depth > depth)
	{
		NazaraError("Cube dimensions are out of bounds");
		return false;
	}
	#endif

	EnsureOwnership();

	nzUInt8 bpp = NzPixelFormat::GetBPP(m_sharedImage->format);
	nzUInt8* dstPixels = GetPixelPtr(m_sharedImage->pixels[level], bpp, cube.x, cube.y, cube.z, width, height);
	unsigned int srcStride = cube.width * bpp;
	unsigned int dstStride = width * bpp;
	unsigned int faceSize = dstStride * height;
	for (unsigned int z = 0; z < cube.depth; ++z)
	{
		nzUInt8* facePixels = dstPixels;
		for (unsigned int y = 0; y < cube.height; ++y)
		{
			std::memcpy(facePixels, pixels, srcStride);
			pixels += srcStride;
			facePixels += dstStride;
		}

		dstPixels += faceSize;
	}

	return true;
}

NzImage& NzImage::operator=(const NzImage& image)
{
	ReleaseImage();

	m_sharedImage = image.m_sharedImage;
	if (m_sharedImage != &emptyImage)
	{
		NazaraMutexLock(m_sharedImage->mutex);
		m_sharedImage->refCount++;
		NazaraMutexUnlock(m_sharedImage->mutex);
	}

	return *this;
}

NzImage& NzImage::operator=(NzImage&& image) noexcept
{
	std::swap(m_sharedImage, image.m_sharedImage);

	return *this;
}

nzUInt8 NzImage::GetMaxLevel(unsigned int width, unsigned int height, unsigned int depth)
{
	static const float l2 = std::log(2.f);

	unsigned int widthLevel = std::log(static_cast<float>(width))/l2;
	unsigned int heightLevel = std::log(static_cast<float>(height))/l2;
	unsigned int depthLevel = std::log(static_cast<float>(depth))/l2;

	return std::max(std::max(std::max(widthLevel, heightLevel), depthLevel), 1U);
}

void NzImage::EnsureOwnership()
{
	if (m_sharedImage == &emptyImage)
		return;

	NazaraLock(m_sharedImage->mutex);
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

	NazaraMutexLock(m_sharedImage->mutex);
	bool freeSharedImage = (--m_sharedImage->refCount == 0);
	NazaraMutexUnlock(m_sharedImage->mutex);

	if (freeSharedImage)
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
