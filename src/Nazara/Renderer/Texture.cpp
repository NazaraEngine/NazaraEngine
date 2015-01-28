// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <memory>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

struct NzTextureImpl
{
	GLuint id;
	nzImageType type;
	nzPixelFormat format;
	nzUInt8 levelCount;
	bool mipmapping = false;
	bool mipmapsUpdated = true;
	unsigned int depth;
	unsigned int height;
	unsigned int width;
};

namespace
{
	inline unsigned int GetLevelSize(unsigned int size, nzUInt8 level)
	{
		// Contrairement à la classe Image, un appel à GetLevelSize(0, level) n'est pas possible
		return std::max(size >> level, 1U);
	}

	inline void SetUnpackAlignement(nzUInt8 bpp)
	{
		if (bpp % 8 == 0)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
		else if (bpp % 4 == 0)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		else if (bpp % 2 == 0)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
		else
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}
}

NzTexture::NzTexture(nzImageType type, nzPixelFormat format, unsigned int width, unsigned int height, unsigned int depth, nzUInt8 levelCount)
{
	NzErrorFlags flags(nzErrorFlag_ThrowException);
	Create(type, format, width, height, depth, levelCount);
}

NzTexture::NzTexture(const NzImage& image)
{
	NzErrorFlags flags(nzErrorFlag_ThrowException);
	LoadFromImage(image);
}

NzTexture::~NzTexture()
{
	Destroy();
	NzRenderer::OnTextureReleased(this);
}

bool NzTexture::Create(nzImageType type, nzPixelFormat format, unsigned int width, unsigned int height, unsigned int depth, nzUInt8 levelCount)
{
	Destroy();

	#if NAZARA_RENDERER_SAFE
	if (!IsTypeSupported(type))
	{
		NazaraError("Texture's type not supported");
		return false;
	}

	if (!NzPixelFormat::IsValid(format))
	{
		NazaraError("Invalid pixel format");
		return false;
	}

	if (!IsFormatSupported(format))
	{
		NazaraError("Texture's format not supported");
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
				NazaraError("One dimensional texture's height must be 1");
				return false;
			}

			if (depth > 1)
			{
				NazaraError("1D textures must be 1 deep");
				return false;
			}
			break;

		case nzImageType_1D_Array:
		case nzImageType_2D:
			if (depth > 1)
			{
				NazaraError("2D textures must be 1 deep");
				return false;
			}
			break;

		case nzImageType_2D_Array:
		case nzImageType_3D:
			break;

		case nzImageType_Cubemap:
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
	}
	#endif

	NzContext::EnsureContext();

	if (IsMipmappingSupported())
		levelCount = std::min(levelCount, NzImage::GetMaxLevel(width, height, depth));
	else if (levelCount > 1)
	{
		NazaraWarning("Mipmapping not supported, reducing level count to 1");
		levelCount = 1;
	}

	m_impl = new NzTextureImpl;
	m_impl->depth = GetValidSize(depth);
	m_impl->format = format;
	m_impl->height = GetValidSize(height);
	m_impl->levelCount = levelCount;
	m_impl->type = type;
	m_impl->width = GetValidSize(width);

	glGenTextures(1, &m_impl->id);
	NzOpenGL::BindTexture(m_impl->type, m_impl->id);

	// En cas d'erreur (sortie prématurée), on détruit la texture
	NzCallOnExit onExit([this]()
	{
		Destroy();
	});

	// On précise le nombre de mipmaps avant la spécification de la texture
	// https://www.opengl.org/wiki/Hardware_specifics:_NVidia
	SetMipmapRange(0, m_impl->levelCount-1);
	if (m_impl->levelCount > 1U)
		EnableMipmapping(true);

	// Vérification du support par la carte graphique (texture proxy)
	if (!CreateTexture(true))
	{
		NazaraError("Texture's parameters not supported by driver");
		return false;
	}

	// Création de la texture
	if (!CreateTexture(false))
	{
		NazaraError("Failed to create texture");
		return false;
	}

	onExit.Reset();

	NotifyCreated();
	return true;
}

void NzTexture::Destroy()
{
	if (m_impl)
	{
		NotifyDestroy();

		NzContext::EnsureContext();
		NzOpenGL::DeleteTexture(m_impl->id);

		delete m_impl;
		m_impl = nullptr;
	}
}

bool NzTexture::Download(NzImage* image) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return false;
	}

	if (!image)
	{
		NazaraError("Image must be valid");
		return false;
	}
	#endif

	NzOpenGL::Format format;
	if (!NzOpenGL::TranslateFormat(m_impl->format, &format, NzOpenGL::FormatType_Texture))
	{
		NazaraError("Failed to get OpenGL format");
		return false;
	}

	if (!image->Create(m_impl->type, m_impl->format, m_impl->width, m_impl->height, m_impl->depth, m_impl->levelCount))
	{
		NazaraError("Failed to create image");
		return false;
	}

	// Téléchargement...
	NzOpenGL::BindTexture(m_impl->type, m_impl->id);
	for (nzUInt8 level = 0; level < m_impl->levelCount; ++level)
		glGetTexImage(NzOpenGL::TextureTarget[m_impl->type], level, format.dataFormat, format.dataType, image->GetPixels(0, 0, 0, level));

	return true;
}

bool NzTexture::EnableMipmapping(bool enable)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return false;
	}
	#endif

	if (!IsMipmappingSupported())
	{
		NazaraError("Mipmapping not supported");
		return false;
	}

	if (m_impl->levelCount == 1) // Transformation d'une texture sans mipmaps vers une texture avec mipmaps
	{
		///FIXME: Est-ce que cette opération est seulement possible ?
		m_impl->levelCount = NzImage::GetMaxLevel(m_impl->width, m_impl->height, m_impl->depth);
		SetMipmapRange(0, m_impl->levelCount-1);
	}

	if (!m_impl->mipmapping && enable)
		m_impl->mipmapsUpdated = false;

	m_impl->mipmapping = enable;

	return true;
}

void NzTexture::EnsureMipmapsUpdate() const
{
	if (m_impl->mipmapping && !m_impl->mipmapsUpdated)
	{
		NzOpenGL::BindTexture(m_impl->type, m_impl->id);
		glGenerateMipmap(NzOpenGL::TextureTarget[m_impl->type]);
		m_impl->mipmapsUpdated = true;
	}
}

unsigned int NzTexture::GetDepth(nzUInt8 level) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return 0;
	}
	#endif

	return GetLevelSize(m_impl->depth, level);
}

nzPixelFormat NzTexture::GetFormat() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return nzPixelFormat_Undefined;
	}
	#endif

	return m_impl->format;
}

unsigned int NzTexture::GetHeight(nzUInt8 level) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return 0;
	}
	#endif

	return GetLevelSize(m_impl->height, level);
}

nzUInt8 NzTexture::GetLevelCount() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return 0;
	}
	#endif

	return m_impl->levelCount;
}

nzUInt8 NzTexture::GetMaxLevel() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return 0;
	}
	#endif

	return NzImage::GetMaxLevel(m_impl->type, m_impl->width, m_impl->height, m_impl->depth);
}

unsigned int NzTexture::GetMemoryUsage() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return 0;
	}
	#endif

	unsigned int width = m_impl->width;
	unsigned int height = m_impl->height;
	unsigned int depth = m_impl->depth;

	unsigned int size = 0;
	for (unsigned int i = 0; i < m_impl->levelCount; ++i)
	{
		size += width * height * depth;

		if (width > 1)
			width >>= 1;

		if (height > 1)
			height >>= 1;

		if (depth > 1)
			depth >>= 1;
	}

	if (m_impl->type == nzImageType_Cubemap)
		size *= 6;

	return size * NzPixelFormat::GetBytesPerPixel(m_impl->format);
}

unsigned int NzTexture::GetMemoryUsage(nzUInt8 level) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return 0;
	}

	if (level >= m_impl->levelCount)
	{
		NazaraError("Level out of bounds (" + NzString::Number(level) + " >= " + NzString::Number(m_impl->levelCount) + ')');
		return 0;
	}
	#endif

	return (GetLevelSize(m_impl->width, level)) *
	       (GetLevelSize(m_impl->height, level)) *
	       ((m_impl->type == nzImageType_Cubemap) ? 6 : GetLevelSize(m_impl->depth, level)) *
	       NzPixelFormat::GetBytesPerPixel(m_impl->format);
}

NzVector3ui NzTexture::GetSize(nzUInt8 level) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return NzVector3ui(0, 0, 0);
	}
	#endif

	return NzVector3ui(GetLevelSize(m_impl->width, level), GetLevelSize(m_impl->height, level), GetLevelSize(m_impl->depth, level));
}

nzImageType NzTexture::GetType() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return nzImageType_2D;
	}
	#endif

	return m_impl->type;
}

unsigned int NzTexture::GetWidth(nzUInt8 level) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return 0;
	}
	#endif

	return GetLevelSize(m_impl->width, level);
}

bool NzTexture::HasMipmaps() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return false;
	}
	#endif

	return m_impl->levelCount > 1;
}

void NzTexture::InvalidateMipmaps()
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraInternalError("Texture must be valid");
		return;
	}
	#endif

	m_impl->mipmapsUpdated = false;
}

bool NzTexture::IsValid() const
{
	return m_impl != nullptr;
}

bool NzTexture::LoadFromFile(const NzString& filePath, const NzImageParams& params, bool generateMipmaps)
{
	NzImage image;
	if (!image.LoadFromFile(filePath, params))
	{
		NazaraError("Failed to load image");
		return false;
	}

	return LoadFromImage(image, generateMipmaps);
}

bool NzTexture::LoadFromImage(const NzImage& image, bool generateMipmaps)
{
	#if NAZARA_RENDERER_SAFE
	if (!image.IsValid())
	{
		NazaraError("Image must be valid");
		return false;
	}
	#endif

	// Vive le Copy-On-Write
	NzImage newImage(image);

	nzPixelFormat format = newImage.GetFormat();
	if (!IsFormatSupported(format))
	{
		///TODO: Sélectionner le format le plus adapté selon les composantes présentes dans le premier format
		nzPixelFormat newFormat = (NzPixelFormat::HasAlpha(format)) ? nzPixelFormat_BGRA8 : nzPixelFormat_BGR8;
		NazaraWarning("Format " + NzPixelFormat::ToString(format) + " not supported, trying to convert it to " + NzPixelFormat::ToString(newFormat) + "...");

		if (NzPixelFormat::IsConversionSupported(format, newFormat))
		{
			if (newImage.Convert(newFormat))
			{
				NazaraWarning("Conversion succeed");
				format = newFormat;
			}
			else
			{
				NazaraError("Conversion failed");
				return false;
			}
		}
		else
		{
			NazaraError("Conversion not supported");
			return false;
		}
	}

	nzImageType type = newImage.GetType();
	nzUInt8 levelCount = newImage.GetLevelCount();
	if (!Create(type, format, newImage.GetWidth(), newImage.GetHeight(), newImage.GetDepth(), (generateMipmaps) ? 0xFF : levelCount))
	{
		NazaraError("Failed to create texture");
		return false;
	}

	NzCallOnExit destroyOnExit([this]()
	{
		Destroy();
	});

	if (type == nzImageType_Cubemap)
	{
		for (nzUInt8 level = 0; level < levelCount; ++level)
		{
			for (unsigned int i = 0; i <= nzCubemapFace_Max; ++i)
			{
				if (!Update(newImage.GetConstPixels(0, 0, i, level), NzRectui(0, 0, newImage.GetWidth(level), newImage.GetHeight(level)), i, level))
				{
					NazaraError("Failed to update texture");
					return false;
				}
			}
		}
	}
	else
	{
		for (nzUInt8 level = 0; level < levelCount; ++level)
		{
			if (!Update(newImage.GetConstPixels(0, 0, 0, level), level))
			{
				NazaraError("Failed to update texture");
				return false;
			}
		}
	}

	destroyOnExit.Reset();

	return true;
}

bool NzTexture::LoadFromMemory(const void* data, std::size_t size, const NzImageParams& params, bool generateMipmaps)
{
	NzImage image;
	if (!image.LoadFromMemory(data, size, params))
	{
		NazaraError("Failed to load image");
		return false;
	}

	return LoadFromImage(image, generateMipmaps);
}

bool NzTexture::LoadFromStream(NzInputStream& stream, const NzImageParams& params, bool generateMipmaps)
{
	NzImage image;
	if (!image.LoadFromStream(stream, params))
	{
		NazaraError("Failed to load image");
		return false;
	}

	return LoadFromImage(image, generateMipmaps);
}

bool NzTexture::LoadArrayFromFile(const NzString& filePath, const NzImageParams& imageParams, bool generateMipmaps, const NzVector2ui& atlasSize)
{
	NzImage cubemap;
	if (!cubemap.LoadArrayFromFile(filePath, imageParams, atlasSize))
	{
		NazaraError("Failed to load cubemap");
		return false;
	}

	return LoadFromImage(cubemap, generateMipmaps);
}

bool NzTexture::LoadArrayFromImage(const NzImage& image, bool generateMipmaps, const NzVector2ui& atlasSize)
{
	NzImage cubemap;
	if (!cubemap.LoadArrayFromImage(image, atlasSize))
	{
		NazaraError("Failed to load cubemap");
		return false;
	}

	return LoadFromImage(cubemap, generateMipmaps);
}

bool NzTexture::LoadArrayFromMemory(const void* data, std::size_t size, const NzImageParams& imageParams, bool generateMipmaps, const NzVector2ui& atlasSize)
{
	NzImage cubemap;
	if (!cubemap.LoadArrayFromMemory(data, size, imageParams, atlasSize))
	{
		NazaraError("Failed to load cubemap");
		return false;
	}

	return LoadFromImage(cubemap, generateMipmaps);
}

bool NzTexture::LoadArrayFromStream(NzInputStream& stream, const NzImageParams& imageParams, bool generateMipmaps, const NzVector2ui& atlasSize)
{
	NzImage cubemap;
	if (!cubemap.LoadArrayFromStream(stream, imageParams, atlasSize))
	{
		NazaraError("Failed to load cubemap");
		return false;
	}

	return LoadFromImage(cubemap, generateMipmaps);
}

bool NzTexture::LoadCubemapFromFile(const NzString& filePath, const NzImageParams& imageParams, bool generateMipmaps, const NzCubemapParams& cubemapParams)
{
	NzImage cubemap;
	if (!cubemap.LoadCubemapFromFile(filePath, imageParams, cubemapParams))
	{
		NazaraError("Failed to load cubemap");
		return false;
	}

	return LoadFromImage(cubemap, generateMipmaps);
}

bool NzTexture::LoadCubemapFromImage(const NzImage& image, bool generateMipmaps, const NzCubemapParams& params)
{
	NzImage cubemap;
	if (!cubemap.LoadCubemapFromImage(image, params))
	{
		NazaraError("Failed to load cubemap");
		return false;
	}

	return LoadFromImage(cubemap, generateMipmaps);
}

bool NzTexture::LoadCubemapFromMemory(const void* data, std::size_t size, const NzImageParams& imageParams, bool generateMipmaps, const NzCubemapParams& cubemapParams)
{
	NzImage cubemap;
	if (!cubemap.LoadCubemapFromMemory(data, size, imageParams, cubemapParams))
	{
		NazaraError("Failed to load cubemap");
		return false;
	}

	return LoadFromImage(cubemap, generateMipmaps);
}

bool NzTexture::LoadCubemapFromStream(NzInputStream& stream, const NzImageParams& imageParams, bool generateMipmaps, const NzCubemapParams& cubemapParams)
{
	NzImage cubemap;
	if (!cubemap.LoadCubemapFromStream(stream, imageParams, cubemapParams))
	{
		NazaraError("Failed to load cubemap");
		return false;
	}

	return LoadFromImage(cubemap, generateMipmaps);
}

bool NzTexture::LoadFaceFromFile(nzCubemapFace face, const NzString& filePath, const NzImageParams& params)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return false;
	}

	if (m_impl->type != nzImageType_Cubemap)
	{
		NazaraError("Texture must be a cubemap");
		return false;
	}
	#endif

	NzImage image;
	if (!image.LoadFromFile(filePath, params))
	{
		NazaraError("Failed to load image");
		return false;
	}

	if (!image.Convert(m_impl->format))
	{
		NazaraError("Failed to convert image to texture format");
		return false;
	}

	unsigned int faceSize = m_impl->width;
	if (image.GetWidth() != faceSize || image.GetHeight() != faceSize)
	{
		NazaraError("Image size must match texture face size");
		return false;
	}

	return Update(image, NzRectui(0, 0, faceSize, faceSize), face);
}

bool NzTexture::LoadFaceFromMemory(nzCubemapFace face, const void* data, std::size_t size, const NzImageParams& params)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return false;
	}

	if (m_impl->type != nzImageType_Cubemap)
	{
		NazaraError("Texture must be a cubemap");
		return false;
	}
	#endif

	NzImage image;
	if (!image.LoadFromMemory(data, size, params))
	{
		NazaraError("Failed to load image");
		return false;
	}

	if (!image.Convert(m_impl->format))
	{
		NazaraError("Failed to convert image to texture format");
		return false;
	}

	unsigned int faceSize = m_impl->width;
	if (image.GetWidth() != faceSize || image.GetHeight() != faceSize)
	{
		NazaraError("Image size must match texture face size");
		return false;
	}

	return Update(image, NzRectui(0, 0, faceSize, faceSize), face);
}

bool NzTexture::LoadFaceFromStream(nzCubemapFace face, NzInputStream& stream, const NzImageParams& params)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return false;
	}

	if (m_impl->type != nzImageType_Cubemap)
	{
		NazaraError("Texture must be a cubemap");
		return false;
	}
	#endif

	NzImage image;
	if (!image.LoadFromStream(stream, params))
	{
		NazaraError("Failed to load image");
		return false;
	}

	if (!image.Convert(m_impl->format))
	{
		NazaraError("Failed to convert image to texture format");
		return false;
	}

	unsigned int faceSize = m_impl->width;

	if (image.GetWidth() != faceSize || image.GetHeight() != faceSize)
	{
		NazaraError("Image size must match texture face size");
		return false;
	}

	return Update(image, NzRectui(0, 0, faceSize, faceSize), face);
}

bool NzTexture::SetMipmapRange(nzUInt8 minLevel, nzUInt8 maxLevel)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return false;
	}

	if (minLevel >= m_impl->levelCount)
	{
		NazaraError("Minimum level cannot be greater or equal than level count (" + NzString::Number(minLevel) + " >= " + NzString::Number(m_impl->levelCount) + ')');
		return false;
	}

	if (maxLevel < minLevel)
	{
		NazaraError("Minimum level cannot be greater than maximum level (" + NzString::Number(minLevel) + " < " + NzString::Number(maxLevel) + ')');
		return false;
	}
	#endif

	NzOpenGL::BindTexture(m_impl->type, m_impl->id);
	glTexParameteri(NzOpenGL::TextureTarget[m_impl->type], GL_TEXTURE_BASE_LEVEL, minLevel);
	glTexParameteri(NzOpenGL::TextureTarget[m_impl->type], GL_TEXTURE_MAX_LEVEL, maxLevel);

	return true;
}

bool NzTexture::Update(const NzImage& image, nzUInt8 level)
{
	#if NAZARA_RENDERER_SAFE
	if (!image.IsValid())
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (image.GetFormat() != m_impl->format)
	{
		NazaraError("Image format does not match texture format");
		return false;
	}
	#endif

	const nzUInt8* pixels = image.GetConstPixels(0, 0, 0, level);
	if (!pixels)
	{
		NazaraError("Failed to access image's pixels");
		return false;
	}

	return Update(pixels, image.GetWidth(level), image.GetHeight(level), level);
}

bool NzTexture::Update(const NzImage& image, const NzBoxui& box, nzUInt8 level)
{
	#if NAZARA_RENDERER_SAFE
	if (!image.IsValid())
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (image.GetFormat() != m_impl->format)
	{
		NazaraError("Image format does not match texture format");
		return false;
	}
	#endif

	const nzUInt8* pixels = image.GetConstPixels(0, 0, 0, level);
	if (!pixels)
	{
		NazaraError("Failed to access image's pixels");
		return false;
	}

	return Update(pixels, box, image.GetWidth(level), image.GetHeight(level), level);
}

bool NzTexture::Update(const NzImage& image, const NzRectui& rect, unsigned int z, nzUInt8 level)
{
	#if NAZARA_RENDERER_SAFE
	if (!image.IsValid())
	{
		NazaraError("Image must be valid");
		return false;
	}

	if (image.GetFormat() != m_impl->format)
	{
		NazaraError("Image format does not match texture format");
		return false;
	}
	#endif

	const nzUInt8* pixels = image.GetConstPixels(0, 0, 0, level);
	if (!pixels)
	{
		NazaraError("Failed to access image's pixels");
		return false;
	}

	return Update(pixels, rect, z, image.GetWidth(level), image.GetHeight(level), level);
}

bool NzTexture::Update(const nzUInt8* pixels, unsigned int srcWidth, unsigned int srcHeight, nzUInt8 level)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return false;
	}
	#endif

	return Update(pixels, NzBoxui(GetLevelSize(m_impl->width, level), GetLevelSize(m_impl->height, level), GetLevelSize(m_impl->depth, level)), srcWidth, srcHeight, level);
}

bool NzTexture::Update(const nzUInt8* pixels, const NzBoxui& box, unsigned int srcWidth, unsigned int srcHeight, nzUInt8 level)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return false;
	}

	if (!pixels)
	{
		NazaraError("Invalid pixel source");
		return false;
	}

	if (!box.IsValid())
	{
		NazaraError("Invalid box");
		return false;
	}
	#endif

	unsigned int height = GetLevelSize(m_impl->height, level);

	#if NAZARA_RENDERER_SAFE
	unsigned int width = GetLevelSize(m_impl->width, level);
	unsigned int depth = (m_impl->type == nzImageType_Cubemap) ? 6 : GetLevelSize(m_impl->depth, level);
	if (box.x+box.width > width || box.y+box.height > height || box.z+box.depth > depth ||
	    (m_impl->type == nzImageType_Cubemap && box.depth > 1)) // Nous n'autorisons pas de modifier plus d'une face du cubemap à la fois
	{
		NazaraError("Cube dimensions are out of bounds");
		return false;
	}

	if (level >= m_impl->levelCount)
	{
		NazaraError("Level out of bounds (" + NzString::Number(level) + " >= " + NzString::Number(m_impl->levelCount) + ')');
		return false;
	}
	#endif

	NzOpenGL::Format format;
	if (!NzOpenGL::TranslateFormat(m_impl->format, &format, NzOpenGL::FormatType_Texture))
	{
		NazaraError("Failed to get OpenGL format");
		return false;
	}

	SetUnpackAlignement(NzPixelFormat::GetBytesPerPixel(m_impl->format));
	glPixelStorei(GL_UNPACK_ROW_LENGTH, srcWidth);
	glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, srcHeight);

	NzOpenGL::BindTexture(m_impl->type, m_impl->id);
	switch (m_impl->type)
	{
		case nzImageType_1D:
			glTexSubImage1D(GL_TEXTURE_1D, level, box.x, box.width, format.dataFormat, format.dataType, pixels);
			break;

		case nzImageType_1D_Array:
		case nzImageType_2D:
			glTexSubImage2D(NzOpenGL::TextureTarget[m_impl->type], level, box.x, box.y, box.width, box.height, format.dataFormat, format.dataType, pixels);
			break;

		case nzImageType_2D_Array:
		case nzImageType_3D:
			glTexSubImage3D(NzOpenGL::TextureTarget[m_impl->type], level, box.x, box.y, box.z, box.width, box.height, box.depth, format.dataFormat, format.dataType, pixels);
			break;

		case nzImageType_Cubemap:
			glTexSubImage2D(NzOpenGL::CubemapFace[box.z], level, box.x, box.y, box.width, box.height, format.dataFormat, format.dataType, pixels);
			break;
	}

	return true;
}

bool NzTexture::Update(const nzUInt8* pixels, const NzRectui& rect, unsigned int z, unsigned int srcWidth, unsigned int srcHeight, nzUInt8 level)
{
	return Update(pixels, NzBoxui(rect.x, rect.y, z, rect.width, rect.height, 1), srcWidth, srcHeight, level);
}

unsigned int NzTexture::GetOpenGLID() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return 0;
	}
	#endif

	return m_impl->id;
}

unsigned int NzTexture::GetValidSize(unsigned int size)
{
	if (NzRenderer::HasCapability(nzRendererCap_TextureNPOT))
		return size;
	else
	{
        unsigned int pot = 1;
        while (pot < size)
            pot <<= 1;

        return pot;
	}
}

bool NzTexture::IsFormatSupported(nzPixelFormat format)
{
	switch (format)
	{
		// Formats de base
		case nzPixelFormat_A8:
		case nzPixelFormat_BGR8:
		case nzPixelFormat_BGRA8:
		case nzPixelFormat_L8:
		case nzPixelFormat_LA8:
		case nzPixelFormat_RGB8:
		case nzPixelFormat_RGBA8:
			return true;

		// Packed formats supportés depuis OpenGL 1.2
		case nzPixelFormat_RGB5A1:
		case nzPixelFormat_RGBA4:
			return true;

		// Formats supportés depuis OpenGL 3
		case nzPixelFormat_R8:
		case nzPixelFormat_R8I:
		case nzPixelFormat_R8UI:
		case nzPixelFormat_R16:
		case nzPixelFormat_R16F:
		case nzPixelFormat_R16I:
		case nzPixelFormat_R16UI:
		case nzPixelFormat_R32F:
		case nzPixelFormat_R32I:
		case nzPixelFormat_R32UI:
		case nzPixelFormat_RG8:
		case nzPixelFormat_RG8I:
		case nzPixelFormat_RG8UI:
		case nzPixelFormat_RG16:
		case nzPixelFormat_RG16F:
		case nzPixelFormat_RG16I:
		case nzPixelFormat_RG16UI:
		case nzPixelFormat_RG32F:
		case nzPixelFormat_RG32I:
		case nzPixelFormat_RG32UI:
		case nzPixelFormat_RGB16F:
		case nzPixelFormat_RGB16I:
		case nzPixelFormat_RGB16UI:
		case nzPixelFormat_RGB32F:
		case nzPixelFormat_RGB32I:
		case nzPixelFormat_RGB32UI:
		case nzPixelFormat_RGBA16F:
		case nzPixelFormat_RGBA16I:
		case nzPixelFormat_RGBA16UI:
		case nzPixelFormat_RGBA32F:
		case nzPixelFormat_RGBA32I:
		case nzPixelFormat_RGBA32UI:
			return NzOpenGL::GetVersion() >= 300;

		// Formats de profondeur (Supportés avec les FBOs)
		case nzPixelFormat_Depth16:
		case nzPixelFormat_Depth24:
		case nzPixelFormat_Depth32:
		case nzPixelFormat_Depth24Stencil8:
			return NzOpenGL::IsSupported(nzOpenGLExtension_FrameBufferObject);

		// Formats de stencil (Non supportés pour les textures)
		case nzPixelFormat_Stencil1:
		case nzPixelFormat_Stencil4:
		case nzPixelFormat_Stencil8:
		case nzPixelFormat_Stencil16:
			return false;

		// Formats compressés
		case nzPixelFormat_DXT1:
		case nzPixelFormat_DXT3:
		case nzPixelFormat_DXT5:
			return NzOpenGL::IsSupported(nzOpenGLExtension_TextureCompression_s3tc);

		case nzPixelFormat_Undefined:
			break;
	}

	NazaraError("Invalid pixel format");
	return false;
}

bool NzTexture::IsMipmappingSupported()
{
	return glGenerateMipmap != nullptr;
}

bool NzTexture::IsTypeSupported(nzImageType type)
{
	switch (type)
	{
		case nzImageType_1D:
		case nzImageType_2D:
		case nzImageType_3D:
		case nzImageType_Cubemap:
			return true; // Tous supportés nativement dans OpenGL 2

		case nzImageType_1D_Array:
		case nzImageType_2D_Array:
			return NzOpenGL::IsSupported(nzOpenGLExtension_TextureArray);
	}

	NazaraError("Image type not handled (0x" + NzString::Number(type, 16) + ')');
	return false;
}

bool NzTexture::CreateTexture(bool proxy)
{
	NzOpenGL::Format openGLFormat;
	if (!NzOpenGL::TranslateFormat(m_impl->format, &openGLFormat, NzOpenGL::FormatType_Texture))
	{
		NazaraError("Format " + NzPixelFormat::ToString(m_impl->format) + " not supported by OpenGL");
		return false;
	}

	GLenum target = (proxy) ? NzOpenGL::TextureTargetProxy[m_impl->type] : NzOpenGL::TextureTarget[m_impl->type];
	switch (m_impl->type)
	{
		case nzImageType_1D:
		{
			if (glTexStorage1D && !proxy) // Les drivers AMD semblent ne pas aimer glTexStorage avec un format proxy
				glTexStorage1D(target, m_impl->levelCount, openGLFormat.internalFormat, m_impl->width);
			else
			{
				unsigned int w = m_impl->width;
				for (nzUInt8 level = 0; level < m_impl->levelCount; ++level)
				{
					glTexImage1D(target, level, openGLFormat.internalFormat, w, 0, openGLFormat.dataFormat, openGLFormat.dataType, nullptr);
					if (w > 1U)
						w >>= 1;
				}
			}
			break;
		}

		case nzImageType_1D_Array:
		case nzImageType_2D:
		{
			if (glTexStorage2D && !proxy)
				glTexStorage2D(target, m_impl->levelCount, openGLFormat.internalFormat, m_impl->width, m_impl->height);
			else
			{
				unsigned int w = m_impl->width;
				unsigned int h = m_impl->height;
				for (nzUInt8 level = 0; level < m_impl->levelCount; ++level)
				{
					glTexImage2D(target, level, openGLFormat.internalFormat, w, h, 0, openGLFormat.dataFormat, openGLFormat.dataType, nullptr);
					if (w > 1U)
						w >>= 1;

					if (h > 1U)
						h >>= 1;
				}
			}
			break;
		}

		case nzImageType_2D_Array:
		case nzImageType_3D:
		{
			if (glTexStorage3D && !proxy)
				glTexStorage3D(target, m_impl->levelCount, openGLFormat.internalFormat, m_impl->width, m_impl->height, m_impl->depth);
			else
			{
				unsigned int w = m_impl->width;
				unsigned int h = m_impl->height;
				unsigned int d = m_impl->depth;
				for (nzUInt8 level = 0; level < m_impl->levelCount; ++level)
				{
					glTexImage3D(target, level, openGLFormat.internalFormat, w, h, d, 0, openGLFormat.dataFormat, openGLFormat.dataType, nullptr);
					if (w > 1U)
						w >>= 1;

					if (h > 1U)
						h >>= 1;

					if (d > 1U)
						d >>= 1;
				}
			}
			break;
		}

		case nzImageType_Cubemap:
		{
			if (glTexStorage2D && !proxy)
				glTexStorage2D(target, m_impl->levelCount, openGLFormat.internalFormat, m_impl->width, m_impl->height);
			else
			{
				unsigned int size = m_impl->width; // Les cubemaps ont une longueur et largeur identique
				for (nzUInt8 level = 0; level < m_impl->levelCount; ++level)
				{
					for (GLenum face : NzOpenGL::CubemapFace)
						glTexImage2D(face, level, openGLFormat.internalFormat, size, size, 0, openGLFormat.dataFormat, openGLFormat.dataType, nullptr);

					if (size > 1U)
						size >>= 1;
				}
			}
			break;
		}
	}

	if (proxy)
	{
		GLint internalFormat = 0;
		glGetTexLevelParameteriv(target, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
		if (internalFormat == 0)
			return false;
	}

	// Application du swizzle
	if (!proxy && NzOpenGL::GetVersion() >= 300)
	{
		glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, openGLFormat.swizzle[0]);
		glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, openGLFormat.swizzle[1]);
		glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, openGLFormat.swizzle[2]);
		glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, openGLFormat.swizzle[3]);
	}

	return true;
}

bool NzTexture::Initialize()
{
	if (!NzTextureLibrary::Initialize())
	{
		NazaraError("Failed to initialise library");
		return false;
	}

	if (!NzTextureManager::Initialize())
	{
		NazaraError("Failed to initialise manager");
		return false;
	}

	return true;
}

void NzTexture::Uninitialize()
{
	NzTextureManager::Uninitialize();
	NzTextureLibrary::Uninitialize();
}

NzTextureLibrary::LibraryMap NzTexture::s_library;
NzTextureManager::ManagerMap NzTexture::s_managerMap;
NzTextureManager::ManagerParams NzTexture::s_managerParameters;
