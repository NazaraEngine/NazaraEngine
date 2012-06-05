// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Image.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/ResourceLoader.hpp>
#include <cmath>
#include <Nazara/Utility/Debug.hpp>

NzImage::NzImage() :
m_sharedImage(&emptyImage)
{
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

NzImage::NzImage(NzImage&& image) :
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
	if (!IsValid())
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

		levels[i] = ptr;

		for (unsigned int d = 0; d < depth; ++d)
		{
			if (!NzPixelFormat::Convert(m_sharedImage->format, format, pixels, &pixels[srcStride], ptr))
			{
				NazaraError("Failed to convert image");
				for (unsigned int j = 0; j <= i; ++j)
					delete[] levels[j];

				delete[] levels;

				return false;
			}

			pixels += srcStride;
			ptr += dstStride;
		}

		if (width > 1)
			width /= 2;

		if (height > 1)
			height /= 2;

		if (depth > 1 && m_sharedImage->type != nzImageType_Cubemap)
			depth /= 2;
	}

	SharedImage* newImage = new SharedImage(1, m_sharedImage->type, format, m_sharedImage->levelCount, levels, m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth);

	ReleaseImage();
	m_sharedImage = newImage;

	return true;
}

bool NzImage::Copy(const NzImage& source, const NzRectui& srcRect, const NzVector2ui& dstPos)
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

	return Update(&source.GetConstPixels()[(srcRect.x + srcRect.y * source.GetHeight()) * source.GetBPP()],
	              NzRectui(dstPos.x, dstPos.y, srcRect.width, srcRect.height));
}

bool NzImage::CopyToFace(nzCubemapFace face, const NzImage& source, const NzRectui& srcRect, const NzVector2ui& dstPos)
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

	return UpdateFace(face,
	                  &source.GetConstPixels()[(srcRect.x + srcRect.y * source.GetHeight()) * source.GetBPP()],
					  NzRectui(dstPos.x, dstPos.y, srcRect.width, srcRect.height));
}

bool NzImage::Create(nzImageType type, nzPixelFormat format, unsigned int width, unsigned int height, unsigned int depth, nzUInt8 levelCount)
{
	ReleaseImage();

	if (width == 0 || height == 0 || depth == 0)
		return true;

	#if NAZARA_UTILITY_SAFE
	if (!NzPixelFormat::IsValid(format))
	{
		NazaraError("Invalid pixel format");
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

		case nzImageType_2D:
			if (depth > 1)
			{
				NazaraError("2D textures must be 1 depth");
				return false;
			}
			break;

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
				w /= 2;

			if (h > 1)
				h /= 2;

			if (d > 1 && type != nzImageType_Cubemap)
				d /= 2;
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to allocate image's level " + NzString::Number(i) + " (" + NzString(e.what()) + ')');
			for (unsigned int j = 0; j <= i; ++j)
				delete[] levels[j];

			delete[] levels;

			return false;
		}
	}

	m_sharedImage = new SharedImage(1, type, format, levelCount, levels, width, height, depth);

	return true;
}

void NzImage::Destroy()
{
	ReleaseImage();
}

nzUInt8 NzImage::GetBPP() const
{
	return NzPixelFormat::GetBPP(m_sharedImage->format);
}

const nzUInt8* NzImage::GetConstPixels(nzUInt8 level) const
{
	return m_sharedImage->pixels[level];
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

	return std::max(m_sharedImage->depth >> level, 1U);
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

	return std::max(m_sharedImage->height >> level, 1U);
}

nzUInt8 NzImage::GetLevelCount() const
{
	return m_sharedImage->levelCount;
}

nzUInt8 NzImage::GetMaxLevel() const
{
	return GetMaxLevel(m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth);
}

NzColor NzImage::GetPixel(unsigned int x, unsigned int y, unsigned int z) const
{
	#if NAZARA_UTILITY_SAFE
	if (!IsValid())
	{
		NazaraError("Image must be valid");
		return NzColor();
	}

	if (m_sharedImage->type == nzImageType_Cubemap)
	{
		NazaraError("GetPixel is not designed for cubemaps, use GetPixelFace instead");
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

	if (z >= m_sharedImage->depth)
	{
		NazaraError("Z value exceeds depth (" + NzString::Number(z) + " >= (" + NzString::Number(m_sharedImage->depth) + ')');
		return NzColor();
	}
	#endif

	const nzUInt8* pixel = &m_sharedImage->pixels[0][(m_sharedImage->height*(m_sharedImage->width*z+y) + x) * NzPixelFormat::GetBPP(m_sharedImage->format)];

	NzColor color;
	if (!NzPixelFormat::Convert(m_sharedImage->format, nzPixelFormat_RGBA8, pixel, &color.r))
		NazaraError("Failed to convert image's format to RGBA8");

	return color;
}

NzColor NzImage::GetPixelFace(nzCubemapFace face, unsigned int x, unsigned int y) const
{
	#if NAZARA_UTILITY_SAFE
	if (!IsValid())
	{
		NazaraError("Image must be valid");
		return NzColor();
	}

	if (m_sharedImage->type != nzImageType_Cubemap)
	{
		NazaraError("GetPixelFace is designed for cubemaps, use GetPixel instead");
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
	#endif

	const nzUInt8* pixel = &m_sharedImage->pixels[0][(m_sharedImage->height*(m_sharedImage->width*(face-nzCubemapFace_PositiveX)+y) + x) * NzPixelFormat::GetBPP(m_sharedImage->format)];

	NzColor color;
	if (!NzPixelFormat::Convert(m_sharedImage->format, nzPixelFormat_RGBA8, pixel, &color.r))
		NazaraError("Failed to convert image's format to RGBA8");

	return color;
}

nzUInt8* NzImage::GetPixels(nzUInt8 level)
{
	EnsureOwnership();

	return m_sharedImage->pixels[level];
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
			width /= 2;

		if (height > 1)
			height /= 2;

		if (depth > 1)
			depth /= 2;
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

	return (std::max(m_sharedImage->width >> level, 1U)) *
	       (std::max(m_sharedImage->height >> level, 1U)) *
	       ((m_sharedImage->type == nzImageType_Cubemap) ? 6 : std::max(m_sharedImage->depth >> level, 1U)) *
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

	return std::max(m_sharedImage->width >> level, 1U);
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
	return NzResourceLoader<NzImage, NzImageParams>::LoadResourceFromFile(this, filePath, params);
}

bool NzImage::LoadFromMemory(const void* data, std::size_t size, const NzImageParams& params)
{
	return NzResourceLoader<NzImage, NzImageParams>::LoadResourceFromMemory(this, data, size, params);
}

bool NzImage::LoadFromStream(NzInputStream& stream, const NzImageParams& params)
{
	return NzResourceLoader<NzImage, NzImageParams>::LoadResourceFromStream(this, stream, params);
}

bool NzImage::SetLevelCount(nzUInt8 levelCount)
{
	#if NAZARA_UTILITY_SAFE
	if (!IsValid())
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

bool NzImage::SetPixel(const NzColor& color, unsigned int x, unsigned int y, unsigned int z)
{
	#if NAZARA_UTILITY_SAFE
	if (!IsValid())
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (m_sharedImage->type == nzImageType_Cubemap)
	{
		NazaraError("SetPixel is not designed for cubemaps, use SetPixelFace instead");
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

	if (z >= m_sharedImage->depth)
	{
		NazaraError("Z value exceeds depth (" + NzString::Number(z) + " >= (" + NzString::Number(m_sharedImage->depth) + ')');
		return false;
	}
	#endif

	nzUInt8* pixel = &m_sharedImage->pixels[0][(m_sharedImage->height*(m_sharedImage->width*z+y) + x) * NzPixelFormat::GetBPP(m_sharedImage->format)];

	if (!NzPixelFormat::Convert(nzPixelFormat_RGBA8, m_sharedImage->format, &color.r, pixel))
	{
		NazaraError("Failed to convert RGBA8 to image's format");
		return false;
	}

	return true;
}

bool NzImage::SetPixelFace(nzCubemapFace face, const NzColor& color, unsigned int x, unsigned int y)
{
	#if NAZARA_UTILITY_SAFE
	if (!IsValid())
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (m_sharedImage->type != nzImageType_Cubemap)
	{
		NazaraError("SetPixelFace is designed for cubemaps, use SetPixel instead");
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
	#endif

	nzUInt8* pixel = &m_sharedImage->pixels[0][(m_sharedImage->height*(m_sharedImage->width*(face-nzCubemapFace_PositiveX)+y) + x) * NzPixelFormat::GetBPP(m_sharedImage->format)];

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
	if (!IsValid())
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (m_sharedImage->type == nzImageType_Cubemap)
	{
		NazaraError("Update is not designed for cubemaps, use UpdateFace instead");
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
	if (!IsValid())
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (m_sharedImage->type == nzImageType_Cubemap)
	{
		NazaraError("Update is not designed for cubemaps, use UpdateFace instead");
		return false;
	}

	if (!pixels)
	{
		NazaraError("Invalid pixel source");
		return false;
	}

	if (!rect.IsValid())
	{
		NazaraError("Invalid rectangle");
		return false;
	}

	if (rect.x+rect.width > std::max(m_sharedImage->width >> level, 1U) || rect.y+rect.height > std::max(m_sharedImage->height >> level, 1U))
	{
		NazaraError("Rectangle dimensions are out of bounds");
		return false;
	}

	if (z >= std::max(m_sharedImage->depth >> level, 1U))
	{
		NazaraError("Z value exceeds depth (" + NzString::Number(z) + " >= (" + NzString::Number(m_sharedImage->depth) + ')');
		return false;
	}

	if (level >= m_sharedImage->levelCount)
	{
		NazaraError("Level out of bounds (" + NzString::Number(level) + " >= " + NzString::Number(m_sharedImage->levelCount) + ')');
		return false;
	}
	#endif

	EnsureOwnership();

	nzUInt8 bpp = NzPixelFormat::GetBPP(m_sharedImage->format);
	nzUInt8* dstPixels = &m_sharedImage->pixels[level][(m_sharedImage->height*(m_sharedImage->width*z + rect.y) + rect.x) * bpp];
	unsigned int srcStride = rect.width * bpp;
	unsigned int blockSize = m_sharedImage->width * bpp;
	for (unsigned int i = 0; i < rect.height; ++i)
	{
		std::memcpy(dstPixels, pixels, blockSize);
		pixels += srcStride;
		dstPixels += blockSize;
	}

	return true;
}

bool NzImage::UpdateFace(nzCubemapFace face, const nzUInt8* pixels, nzUInt8 level)
{
	#if NAZARA_UTILITY_SAFE
	if (!IsValid())
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (m_sharedImage->type != nzImageType_Cubemap)
	{
		NazaraError("UpdateFace is designed for cubemaps, use Update instead");
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

	unsigned int size = GetSize(level);
	std::memcpy(&m_sharedImage->pixels[level][size*(face-nzCubemapFace_PositiveX)], pixels, size);

	return true;
}

bool NzImage::UpdateFace(nzCubemapFace face, const nzUInt8* pixels, const NzRectui& rect, nzUInt8 level)
{
	#if NAZARA_UTILITY_SAFE
	if (!IsValid())
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (m_sharedImage->type != nzImageType_Cubemap)
	{
		NazaraError("UpdateFace is designed for cubemaps, use Update instead");
		return false;
	}

	if (!pixels)
	{
		NazaraError("Invalid pixel source");
		return false;
	}

	if (!rect.IsValid())
	{
		NazaraError("Invalid rectangle");
		return false;
	}

	if (rect.x+rect.width > std::max(m_sharedImage->width >> level, 1U) || rect.y+rect.height > std::max(m_sharedImage->height >> level, 1U))
	{
		NazaraError("Rectangle dimensions are out of bounds");
		return false;
	}

	if (level >= m_sharedImage->levelCount)
	{
		NazaraError("Level out of bounds (" + NzString::Number(level) + " >= " + NzString::Number(m_sharedImage->levelCount) + ')');
		return false;
	}
	#endif

	EnsureOwnership();

	nzUInt8 bpp = NzPixelFormat::GetBPP(m_sharedImage->format);
	nzUInt8* dstPixels = &m_sharedImage->pixels[level][(m_sharedImage->height*(m_sharedImage->width*(face-nzCubemapFace_PositiveX) + rect.y) + rect.x) * bpp];
	unsigned int srcStride = rect.width * bpp;
	unsigned int blockSize = m_sharedImage->width * bpp;
	for (unsigned int i = 0; i < rect.height; ++i)
	{
		std::memcpy(dstPixels, pixels, blockSize);
		pixels += srcStride;
		dstPixels += blockSize;
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

NzImage& NzImage::operator=(NzImage&& image)
{
	std::swap(m_sharedImage, image.m_sharedImage);

	return *this;
}

nzUInt8 NzImage::GetMaxLevel(unsigned int width, unsigned int height, unsigned int depth)
{
	static const float l2 = std::log(2);

	unsigned int widthLevel = std::log(width)/l2;
	unsigned int heightLevel = std::log(height)/l2;
	unsigned int depthLevel = std::log(depth)/l2;

	return std::max(std::max(std::max(widthLevel, heightLevel), depthLevel), 1U);
}

void NzImage::RegisterFileLoader(const NzString& extensions, LoadFileFunction loadFile)
{
	return RegisterResourceFileLoader(extensions, loadFile);
}

void NzImage::RegisterMemoryLoader(IsMemoryLoadingSupportedFunction isLoadingSupported, LoadMemoryFunction loadMemory)
{
	return RegisterResourceMemoryLoader(isLoadingSupported, loadMemory);
}

void NzImage::RegisterStreamLoader(IsStreamLoadingSupportedFunction isLoadingSupported, LoadStreamFunction loadStream)
{
	return RegisterResourceStreamLoader(isLoadingSupported, loadStream);
}

void NzImage::UnregisterFileLoader(const NzString& extensions, LoadFileFunction loadFile)
{
	UnregisterResourceFileLoader(extensions, loadFile);
}

void NzImage::UnregisterMemoryLoader(IsMemoryLoadingSupportedFunction isLoadingSupported, LoadMemoryFunction loadMemory)
{
	UnregisterResourceMemoryLoader(isLoadingSupported, loadMemory);
}

void NzImage::UnregisterStreamLoader(IsStreamLoadingSupportedFunction isLoadingSupported, LoadStreamFunction loadStream)
{
	UnregisterResourceStreamLoader(isLoadingSupported, loadStream);
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
			std::memcpy(pixels[i], &m_sharedImage->pixels[i], size);
		}

		m_sharedImage = new SharedImage(1, m_sharedImage->type, m_sharedImage->format, m_sharedImage->levelCount, pixels, m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth);
	}
}

void NzImage::ReleaseImage()
{
	if (m_sharedImage == &emptyImage)
		return;

	NazaraMutexLock(m_sharedImage->mutex);
	m_sharedImage->refCount--;
	NazaraMutexUnlock(m_sharedImage->mutex);

	if (m_sharedImage->refCount == 0)
	{
		for (unsigned int i = 0; i < m_sharedImage->levelCount; ++i)
			delete[] m_sharedImage->pixels[i];

		delete[] m_sharedImage->pixels;
		delete m_sharedImage;
	}

	m_sharedImage = &emptyImage;
}

NzImage::SharedImage NzImage::emptyImage(0, nzImageType_2D, nzPixelFormat_Undefined, 1, nullptr, 0, 0, 0);
