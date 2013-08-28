// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <memory>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

struct NzTextureImpl
{
	GLuint id;
	nzImageType type;
	nzPixelFormat format;
	nzUInt8 levelCount;
	NzRenderTexture* renderTexture = nullptr;
	bool mipmapping = false;
	bool mipmapsUpdated = true;
	unsigned int depth;
	unsigned int height;
	unsigned int width;
};

namespace
{
	bool CreateTexture(NzTextureImpl* impl, bool proxy)
	{
		NzOpenGL::Format openGLFormat;
		if (!NzOpenGL::TranslateFormat(impl->format, &openGLFormat, NzOpenGL::FormatType_Texture))
		{
			NazaraError("Format " + NzPixelFormat::ToString(impl->format) + " not supported by OpenGL");
			return false;
		}

		GLenum target = (proxy) ? NzOpenGL::TextureTargetProxy[impl->type] : NzOpenGL::TextureTarget[impl->type];
		switch (impl->type)
		{
			case nzImageType_1D:
			{
				if (glTexStorage1D)
					glTexStorage1D(target, impl->levelCount, openGLFormat.internalFormat, impl->width);
				else
				{
					unsigned int w = impl->width;
					for (nzUInt8 level = 0; level < impl->levelCount; ++level)
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
				if (glTexStorage2D)
					glTexStorage2D(target, impl->levelCount, openGLFormat.internalFormat, impl->width, impl->height);
				else
				{
					unsigned int w = impl->width;
					unsigned int h = impl->height;
					for (nzUInt8 level = 0; level < impl->levelCount; ++level)
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
				if (glTexStorage3D)
					glTexStorage3D(target, impl->levelCount, openGLFormat.internalFormat, impl->width, impl->height, impl->depth);
				else
				{
					unsigned int w = impl->width;
					unsigned int h = impl->height;
					unsigned int d = impl->depth;
					for (nzUInt8 level = 0; level < impl->levelCount; ++level)
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
				/*if (glTexStorage2D)
					glTexStorage2D(target, impl->levelCount, openGLFormat.internalFormat, impl->width, impl->height);
				else*/
				{
					unsigned int size = impl->width; // Les cubemaps ont une longueur et largeur identique
					for (nzUInt8 level = 0; level < impl->levelCount; ++level)
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

		return true;
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

NzTexture::NzTexture(const NzImage& image)
{
	LoadFromImage(image);

	#ifdef NAZARA_DEBUG
	if (!m_impl)
	{
		NazaraError("Failed to create texture");
		throw std::runtime_error("Constructor failed");
	}
	#endif
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

	std::unique_ptr<NzTextureImpl> impl(new NzTextureImpl);
	glGenTextures(1, &impl->id);

	impl->depth = GetValidSize(depth);
	impl->format = format;
	impl->height = GetValidSize(height);
	impl->levelCount = levelCount;
	impl->type = type;
	impl->width = GetValidSize(width);

	NzOpenGL::BindTexture(impl->type, impl->id);

	// Vérification du support par la carte graphique
	/*if (!CreateTexture(impl, true))
	{
		NzOpenGL::DeleteTexture(m_impl->id);

		NazaraError("Texture's parameters not supported by driver");
		return false;
	}*/

	// Création de la texture
	if (!CreateTexture(impl.get(), false))
	{
		NzOpenGL::DeleteTexture(m_impl->id);

		NazaraError("Failed to create texture");
		return false;
	}

	m_impl = impl.release();

	if (m_impl->levelCount > 1U)
		EnableMipmapping(true);

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

	unsigned int width = m_impl->width;
	unsigned int height = m_impl->height;
	unsigned int depth = m_impl->depth;

	// Téléchargement...
	NzOpenGL::BindTexture(m_impl->type, m_impl->id);
	for (nzUInt8 level = 0; level < m_impl->levelCount; ++level)
	{
		glGetTexImage(NzOpenGL::TextureTarget[m_impl->type], level, format.dataFormat, format.dataType, image->GetPixels(level));

		if (width > 1)
			width >>= 1;

		if (height > 1)
			height >>= 1;

		if (depth > 1)
			depth >>= 1;
	}

	// Inversion de la texture pour le repère d'OpenGL
	if (!image->FlipVertically())
		NazaraWarning("Failed to flip image");

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
		m_impl->levelCount = NzImage::GetMaxLevel(m_impl->width, m_impl->height, m_impl->depth);

	if (!m_impl->mipmapping && enable)
		m_impl->mipmapsUpdated = false;

	m_impl->mipmapping = enable;

	return true;
}

nzUInt8 NzTexture::GetBytesPerPixel() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return 0;
	}
	#endif

	return NzPixelFormat::GetBytesPerPixel(m_impl->format);
}

unsigned int NzTexture::GetDepth() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return 0;
	}
	#endif

	return m_impl->depth;
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

unsigned int NzTexture::GetHeight() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return 0;
	}
	#endif

	return m_impl->height;
}

NzVector2ui NzTexture::GetSize() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return 0;
	}
	#endif

	return NzVector2ui(m_impl->width, m_impl->height);
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

unsigned int NzTexture::GetWidth() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return 0;
	}
	#endif

	return m_impl->width;
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

bool NzTexture::IsCompressed() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return false;
	}
	#endif

	return NzPixelFormat::IsCompressed(m_impl->format);
}

bool NzTexture::IsCubemap() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return false;
	}
	#endif

	return m_impl->type == nzImageType_Cubemap;
}

bool NzTexture::IsTarget() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return false;
	}
	#endif

	return m_impl->renderTexture != nullptr;
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

	if (type == nzImageType_Cubemap)
	{
		for (nzUInt8 level = 0; level < levelCount; ++level)
		{
			for (unsigned int i = 0; i <= nzCubemapFace_Max; ++i)
			{
				if (!UpdateFace(static_cast<nzCubemapFace>(i), newImage.GetConstPixels(0, 0, i, level), level))
				{
					NazaraError("Failed to update texture");
					Destroy();

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
				Destroy();

				return false;
			}
		}
	}

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

bool NzTexture::LoadCubemapFromFile(const NzString& filePath, const NzImageParams& imageParams, bool generateMipmaps, const NzCubemapParams& cubemapParams)
{
	NzImage image;
	if (!image.LoadFromFile(filePath, imageParams))
	{
		NazaraError("Failed to load image");
		return false;
	}

	return LoadCubemapFromImage(image, generateMipmaps, cubemapParams);
}

bool NzTexture::LoadCubemapFromImage(const NzImage& image, bool generateMipmaps, const NzCubemapParams& params)
{
	// Implémentation presque identique à celle de Image::LoadCubemapFromImage
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

	if (!Create(nzImageType_Cubemap, image.GetFormat(), faceSize, faceSize, 1, (generateMipmaps) ? 0xFF : 1))
	{
		NazaraError("Failed to create texture");
		return false;
	}

	UpdateFace(nzCubemapFace_NegativeX, image.GetConstPixels(leftPos.x, leftPos.y), width, height);
	UpdateFace(nzCubemapFace_NegativeY, image.GetConstPixels(downPos.x, downPos.y), width, height);
	UpdateFace(nzCubemapFace_NegativeZ, image.GetConstPixels(backPos.x, backPos.y), width, height);
	UpdateFace(nzCubemapFace_PositiveX, image.GetConstPixels(rightPos.x, rightPos.y), width, height);
	UpdateFace(nzCubemapFace_PositiveY, image.GetConstPixels(upPos.x, upPos.y), width, height);
	UpdateFace(nzCubemapFace_PositiveZ, image.GetConstPixels(forwardPos.x, forwardPos.y), width, height);

	return true;
}

bool NzTexture::LoadCubemapFromMemory(const void* data, std::size_t size, const NzImageParams& imageParams, bool generateMipmaps, const NzCubemapParams& cubemapParams)
{
	NzImage image;
	if (!image.LoadFromMemory(data, size, imageParams))
	{
		NazaraError("Failed to load image");
		return false;
	}

	return LoadCubemapFromImage(image, generateMipmaps, cubemapParams);
}

bool NzTexture::LoadCubemapFromStream(NzInputStream& stream, const NzImageParams& imageParams, bool generateMipmaps, const NzCubemapParams& cubemapParams)
{
	NzImage image;
	if (!image.LoadFromStream(stream, imageParams))
	{
		NazaraError("Failed to load image");
		return false;
	}

	return LoadCubemapFromImage(image, generateMipmaps, cubemapParams);
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

	return UpdateFace(face, image);
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

	return UpdateFace(face, image);
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

	return UpdateFace(face, image);
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

	const nzUInt8* pixels = image.GetConstPixels(box.x, box.y, box.z, level);
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

	const nzUInt8* pixels = image.GetConstPixels(rect.x, rect.y, z, level);
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

	return Update(pixels, NzBoxui(std::max(m_impl->width >> level, 1U), std::max(m_impl->height >> level, 1U), std::max(m_impl->depth >> level, 1U)), srcWidth, srcHeight, level);
}

bool NzTexture::Update(const nzUInt8* pixels, const NzBoxui& box, unsigned int srcWidth, unsigned int srcHeight, nzUInt8 level)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return false;
	}

	if (m_impl->renderTexture)
	{
		NazaraError("Texture is a target, it cannot be updated");
		return false;
	}

	if (m_impl->type == nzImageType_Cubemap)
	{
		NazaraError("Update is not designed for cubemaps, use UpdateFace instead");
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

	unsigned int height = std::max(m_impl->height >> level, 1U);

	#if NAZARA_RENDERER_SAFE
	if (box.x+box.width > std::max(m_impl->width >> level, 1U) ||
	    box.y+box.height > height ||
		box.z+box.depth > std::max(m_impl->depth >> level, 1U))
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

	nzUInt8 bpp = NzPixelFormat::GetBytesPerPixel(m_impl->format);

	unsigned int size = box.width*box.height*box.depth*bpp;
	std::unique_ptr<nzUInt8[]> flipped(new nzUInt8[size]);
	NzImage::Copy(flipped.get(), pixels, bpp, box.width, box.height, box.depth, 0, 0, srcWidth, srcHeight);

	// Inversion de la texture pour le repère d'OpenGL
	if (!NzPixelFormat::Flip(nzPixelFlipping_Horizontally, m_impl->format, box.width, box.height, box.depth, flipped.get(), flipped.get()))
		NazaraWarning("Failed to flip image");

	SetUnpackAlignement(bpp);

	NzOpenGL::BindTexture(m_impl->type, m_impl->id);
	switch (m_impl->type)
	{
		case nzImageType_1D:
			glTexSubImage1D(GL_TEXTURE_1D, level, box.x, box.width, format.dataFormat, format.dataType, flipped.get());
			break;

		case nzImageType_1D_Array:
		case nzImageType_2D:
			glTexSubImage2D(NzOpenGL::TextureTarget[m_impl->type], level, box.x, height-box.height-box.y, box.width, box.height, format.dataFormat, format.dataType, flipped.get());
			break;

		case nzImageType_2D_Array:
		case nzImageType_3D:
			glTexSubImage3D(NzOpenGL::TextureTarget[m_impl->type], level, box.x, height-box.height-box.y, box.z, box.width, box.height, box.depth, format.dataFormat, format.dataType, flipped.get());
			break;

		case nzImageType_Cubemap:
			NazaraError("Update used on a cubemap texture, please enable safe mode");
			break;
	}

	return true;
}

bool NzTexture::Update(const nzUInt8* pixels, const NzRectui& rect, unsigned int z, unsigned int srcWidth, unsigned int srcHeight, nzUInt8 level)
{
	return Update(pixels, NzBoxui(rect.x, rect.y, z, rect.width, rect.height, 1), srcWidth, srcHeight, level);
}

bool NzTexture::UpdateFace(nzCubemapFace face, const NzImage& image, nzUInt8 level)
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

	return UpdateFace(face, image.GetConstPixels(0, 0, 0, level), NzRectui(0, 0, image.GetWidth(level), image.GetHeight(level)), 0, 0, level);
}

bool NzTexture::UpdateFace(nzCubemapFace face, const NzImage& image, const NzRectui& rect, nzUInt8 level)
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

	return UpdateFace(face, image.GetConstPixels(0, 0, 0, level), rect, image.GetWidth(level), image.GetHeight(level), level);
}

bool NzTexture::UpdateFace(nzCubemapFace face, const nzUInt8* pixels, unsigned int srcWidth, unsigned int srcHeight, nzUInt8 level)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return false;
	}
	#endif

	return UpdateFace(face, pixels, NzRectui(0, 0, m_impl->width, m_impl->height), srcWidth, srcHeight, level);
}

bool NzTexture::UpdateFace(nzCubemapFace face, const nzUInt8* pixels, const NzRectui& rect, unsigned int srcWidth, unsigned int srcHeight, nzUInt8 level)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Texture must be valid");
		return false;
	}

	if (m_impl->renderTexture)
	{
		NazaraError("Texture is a target, it cannot be updated");
		return false;
	}

	if (m_impl->type != nzImageType_Cubemap)
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
	#endif

	unsigned int height = std::max(m_impl->height >> level, 1U);

	#if NAZARA_RENDERER_SAFE
	if (rect.x+rect.width > std::max(m_impl->width >> level, 1U) || rect.y+rect.height > height)
	{
		NazaraError("Rectangle dimensions are out of bounds");
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

	nzUInt8 bpp = NzPixelFormat::GetBytesPerPixel(m_impl->format);

	// Inversion de la texture pour le repère d'OpenGL
	unsigned int size = rect.width*rect.height*bpp;
	std::unique_ptr<nzUInt8[]> flipped(new nzUInt8[size]);
	NzImage::Copy(flipped.get(), pixels, bpp, rect.width, rect.height, 1, 0, 0, srcWidth, srcHeight);

	if (!NzPixelFormat::Flip(nzPixelFlipping_Horizontally, m_impl->format, rect.width, rect.height, 1, flipped.get(), flipped.get()))
		NazaraWarning("Failed to flip image");

	SetUnpackAlignement(bpp);

	NzOpenGL::BindTexture(m_impl->type, m_impl->id);
	glTexSubImage2D(NzOpenGL::CubemapFace[face], level, rect.x, height-rect.height-rect.y, rect.width, rect.height, format.dataFormat, format.dataType, flipped.get());

	return true;
}

bool NzTexture::Bind() const
{
	NzOpenGL::BindTexture(m_impl->type, m_impl->id);

	if (m_impl->mipmapping && !m_impl->mipmapsUpdated)
	{
		glGenerateMipmap(NzOpenGL::TextureTarget[m_impl->type]);
		m_impl->mipmapsUpdated = true;
	}

	return true;
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
		case nzPixelFormat_BGR8:
		case nzPixelFormat_BGRA8:
		case nzPixelFormat_RGB8:
		case nzPixelFormat_RGBA8:
			return true;

		// Packed formats supportés depuis OpenGL 1.2
		case nzPixelFormat_RGB5A1:
		case nzPixelFormat_RGBA4:
			return true;

		// Formats supportés depuis OpenGL 3
		case nzPixelFormat_RGB16F:
		case nzPixelFormat_RGB16I:
		case nzPixelFormat_RGB32F:
		case nzPixelFormat_RGB32I:
		case nzPixelFormat_RGBA16F:
		case nzPixelFormat_RGBA16I:
		case nzPixelFormat_RGBA32F:
		case nzPixelFormat_RGBA32I:
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

		// Dépréciés depuis OpenGL 3
		///FIXME: Il doit bien exister des remplaçants (GL_RED ?)
		case nzPixelFormat_L8:
		case nzPixelFormat_LA8:
			return false;

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

NzRenderTexture* NzTexture::GetRenderTexture() const
{
	#ifdef NAZARA_DEBUG
	if (!m_impl)
	{
		NazaraInternalError("Texture must be valid");
		return nullptr;
	}
	#endif

	return m_impl->renderTexture;
}

void NzTexture::SetRenderTexture(NzRenderTexture* renderTexture)
{
	#ifdef NAZARA_DEBUG
	if (!m_impl)
	{
		NazaraInternalError("Texture must be valid");
		return;
	}
	#endif

	m_impl->renderTexture = renderTexture;
}
