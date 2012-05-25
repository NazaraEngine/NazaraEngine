// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Image.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/ResourceLoader.hpp>
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
	#endif

	if (format == m_sharedImage->format)
		return true;

	// Les images 3D et cubemaps sont stockés de la même façon
	unsigned int depth = (m_sharedImage->type == nzImageType_Cubemap) ? 6 : m_sharedImage->depth;
	unsigned int pixelsPerFace = m_sharedImage->width*m_sharedImage->height;

	nzUInt8* buffer = new nzUInt8[pixelsPerFace*depth*NzPixelFormat::GetBPP(format)];
	nzUInt8* ptr = buffer;
	nzUInt8* pixels = m_sharedImage->pixels;
	unsigned int srcStride = pixelsPerFace * NzPixelFormat::GetBPP(m_sharedImage->format);
	unsigned int dstStride = pixelsPerFace * NzPixelFormat::GetBPP(format);

	for (unsigned int i = 0; i < depth; ++i)
	{
		if (!NzPixelFormat::Convert(m_sharedImage->format, format, pixels, &pixels[srcStride], ptr))
		{
			NazaraError("Failed to convert image");
			delete[] buffer;

			return false;
		}

		pixels += srcStride;
		ptr += dstStride;
	}

	SharedImage* newImage = new SharedImage(1, m_sharedImage->type, format, buffer, m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth);

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

	return Update(&source.GetConstPixels()[(srcRect.x + srcRect.y * source.GetHeight()) * source.GetBPP()], NzRectui(dstPos.x, dstPos.y, srcRect.width, srcRect.height));
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

	return UpdateFace(face, &source.GetConstPixels()[(srcRect.x + srcRect.y * source.GetHeight()) * source.GetBPP()], NzRectui(dstPos.x, dstPos.y, srcRect.width, srcRect.height));
}

bool NzImage::Create(nzImageType type, nzPixelFormat format, unsigned int width, unsigned int height, unsigned int depth)
{
	ReleaseImage();

	#if NAZARA_UTILITY_SAFE
	if (!NzPixelFormat::IsValid(format))
	{
		NazaraError("Invalid pixel format");
		return false;
	}
	#endif

	unsigned int size = width*height*depth*NzPixelFormat::GetBPP(format);
	if (size == 0)
		return true;

	#if NAZARA_UTILITY_SAFE
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

			size *= 6; // Les cubemaps ont six faces
			break;

		default:
			break;
	}
	#else
	if (type == nzImageType_Cubemap)
		size *= 6; // Les cubemaps ont six faces
	#endif

	// Cette allocation est protégée car sa taille dépend directement de paramètres utilisateurs
	nzUInt8* buffer;
	try
	{
		buffer = new nzUInt8[size];
	}
	catch (const std::exception& e)
	{
		NazaraError("Failed to allocate image buffer (" + NzString(e.what()) + ')');
		return false;
	}

	m_sharedImage = new SharedImage(1, type, format, buffer, width, height, depth);

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

const nzUInt8* NzImage::GetConstPixels() const
{
	return m_sharedImage->pixels;
}

unsigned int NzImage::GetDepth() const
{
	return m_sharedImage->depth;
}

nzPixelFormat NzImage::GetFormat() const
{
	return m_sharedImage->format;
}

unsigned int NzImage::GetHeight() const
{
	return m_sharedImage->height;
}

nzUInt8* NzImage::GetPixels()
{
	EnsureOwnership();

	return m_sharedImage->pixels;
}

unsigned int NzImage::GetSize() const
{
	return m_sharedImage->width * m_sharedImage->height * m_sharedImage->depth * NzPixelFormat::GetBPP(m_sharedImage->format);
}

nzImageType	NzImage::GetType() const
{
	return m_sharedImage->type;
}

unsigned int NzImage::GetWidth() const
{
	return m_sharedImage->width;
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

bool NzImage::Update(const nzUInt8* pixels)
{
	#if NAZARA_UTILITY_SAFE
	if (!IsValid())
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (IsCubemap())
	{
		NazaraError("Update is not designed for cubemaps, use UpdateFace instead");
		return false;
	}

	if (!pixels)
	{
		NazaraError("Invalid pixel source");
		return false;
	}
	#endif

	EnsureOwnership();

	std::memcpy(m_sharedImage->pixels, pixels, GetSize());

	return true;
}

bool NzImage::Update(const nzUInt8* pixels, const NzRectui& rect)
{
	#if NAZARA_UTILITY_SAFE
	if (!IsValid())
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (IsCubemap())
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

	if (rect.width > m_sharedImage->width || rect.height > m_sharedImage->height)
	{
		NazaraError("Rectangle dimensions are out of bounds");
		return false;
	}
	#endif

	EnsureOwnership();

	nzUInt8 bpp = NzPixelFormat::GetBPP(m_sharedImage->format);

	nzUInt8* dstPixels = m_sharedImage->pixels + (rect.x + rect.y * m_sharedImage->width) * bpp;
	unsigned int srcStride = rect.width * bpp;
	unsigned int dstStride = m_sharedImage->width * bpp;

	unsigned int blockSize = m_sharedImage->width * bpp;
	for (unsigned int i = 0; i < rect.height; ++i)
	{
		std::memcpy(dstPixels, pixels, blockSize);
		pixels += srcStride;
		dstPixels += dstStride;
	}

	return true;
}

bool NzImage::UpdateFace(nzCubemapFace face, const nzUInt8* pixels)
{
	#if NAZARA_UTILITY_SAFE
	if (!IsValid())
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (!IsCubemap())
	{
		NazaraError("Update is only designed for cubemaps, use Update instead");
		return false;
	}

	if (!pixels)
	{
		NazaraError("Invalid pixel source");
		return false;
	}
	#endif

	EnsureOwnership();

	unsigned int size = GetSize();
	std::memcpy(&m_sharedImage->pixels[size*(face-nzCubemapFace_PositiveX)], pixels, size);

	return true;
}

bool NzImage::UpdateFace(nzCubemapFace face, const nzUInt8* pixels, const NzRectui& rect)
{
	#if NAZARA_UTILITY_SAFE
	if (!IsValid())
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (!IsCubemap())
	{
		NazaraError("Update is only designed for cubemaps, use Update instead");
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

	if (rect.width > m_sharedImage->width || rect.height > m_sharedImage->height)
	{
		NazaraError("Rectangle dimensions are out of bounds");
		return false;
	}
	#endif

	EnsureOwnership();

	nzUInt8 bpp = NzPixelFormat::GetBPP(m_sharedImage->format);

	nzUInt8* dstPixels = m_sharedImage->pixels + (rect.x + rect.y * m_sharedImage->width + (face-nzCubemapFace_PositiveX)*m_sharedImage->width*m_sharedImage->height) * bpp;
	unsigned int srcStride = rect.width * bpp;
	unsigned int dstStride = m_sharedImage->width * bpp;

	unsigned int blockSize = m_sharedImage->width * bpp;
	for (unsigned int i = 0; i < rect.height; ++i)
	{
		std::memcpy(dstPixels, pixels, blockSize);
		pixels += srcStride;
		dstPixels += dstStride;
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

		unsigned int size = GetSize();

		nzUInt8* pixels = new nzUInt8[size];
		std::memcpy(pixels, m_sharedImage->pixels, size);

		m_sharedImage = new SharedImage(1, m_sharedImage->type, m_sharedImage->format, pixels, m_sharedImage->width, m_sharedImage->height, m_sharedImage->depth);
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
		delete[] m_sharedImage->pixels;
		delete m_sharedImage;
	}

	m_sharedImage = &emptyImage;
}

NzImage::SharedImage NzImage::emptyImage(0, nzImageType_2D, nzPixelFormat_Undefined, nullptr, 0, 0, 0);
