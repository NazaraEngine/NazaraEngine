// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderWindow.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

struct NzTextureImpl
{
	GLuint id;
	nzImageType type;
	nzPixelFormat format;
	nzUInt8 levelCount;
	bool isTarget = false;
	bool mipmapping = false;
	bool mipmapsUpdated = true;
	unsigned int depth;
	unsigned int height;
	unsigned int width;
};

namespace
{
	GLenum cubemapFace[] =
	{
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, // nzCubemapFace_PositiveX
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X, // nzCubemapFace_NegativeX
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, // nzCubemapFace_PositiveY
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, // nzCubemapFace_NegativeY
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, // nzCubemapFace_PositiveZ
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z  // nzCubemapFace_NegativeZ
	};

	GLenum openglTarget[] =
	{
		GL_TEXTURE_1D,       // nzImageType_1D
		GL_TEXTURE_1D_ARRAY, // nzImageType_1D_Array
		GL_TEXTURE_2D,       // nzImageType_2D
		GL_TEXTURE_2D_ARRAY, // nzImageType_2D_Array
		GL_TEXTURE_3D,       // nzImageType_3D
		GL_TEXTURE_CUBE_MAP  // nzImageType_Cubemap
	};

	GLenum openglTargetBinding[] =
	{
		GL_TEXTURE_BINDING_1D,       // nzImageType_1D
		GL_TEXTURE_BINDING_1D_ARRAY, // nzImageType_1D
		GL_TEXTURE_BINDING_2D,       // nzImageType_2D
		GL_TEXTURE_BINDING_3D,       // nzImageType_3D
		GL_TEXTURE_BINDING_CUBE_MAP  // nzImageType_Cubemap
	};

	GLenum openglTargetProxy[] =
	{
		GL_PROXY_TEXTURE_1D,       // nzImageType_1D
		GL_PROXY_TEXTURE_1D_ARRAY, // nzImageType_1D_Array
		GL_PROXY_TEXTURE_2D,       // nzImageType_2D
		GL_PROXY_TEXTURE_2D_ARRAY, // nzImageType_2D_Array
		GL_PROXY_TEXTURE_3D,       // nzImageType_3D
		GL_PROXY_TEXTURE_CUBE_MAP  // nzImageType_Cubemap
	};

	struct OpenGLFormat
	{
		GLint internalFormat;
		GLenum dataFormat;
		GLenum dataType;
	};

	bool GetOpenGLFormat(nzPixelFormat pixelFormat, OpenGLFormat* format)
	{
		switch (pixelFormat)
		{
			case nzPixelFormat_BGR8:
				format->dataFormat = GL_BGR;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_RGB8;
				return true;

			case nzPixelFormat_BGRA8:
				format->dataFormat = GL_BGRA;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_RGBA8;
				return true;

			case nzPixelFormat_DXT1:
				format->dataFormat = GL_RGB;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
				return true;

			case nzPixelFormat_DXT3:
				format->dataFormat = GL_RGBA;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				return true;

			case nzPixelFormat_DXT5:
				format->dataFormat = GL_RGBA;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				return true;

			case nzPixelFormat_L8:
			case nzPixelFormat_LA8:
				NazaraError("Pixel format not supported");
				return false;

			case nzPixelFormat_RGB5A1:
				format->dataFormat = GL_RGBA;
				format->dataType = GL_UNSIGNED_SHORT_5_5_5_1;
				format->internalFormat = GL_RGB5_A1;
				return true;

			case nzPixelFormat_RGB8:
				format->dataFormat = GL_RGB;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_RGB8;
				return true;

			case nzPixelFormat_RGBA4:
				format->dataFormat = GL_RGBA;
				format->dataType = GL_UNSIGNED_SHORT_4_4_4_4;
				format->internalFormat = GL_RGBA4;
				return true;

			case nzPixelFormat_RGBA8:
				format->dataFormat = GL_RGBA;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_RGBA8;
				return true;

			case nzPixelFormat_Undefined:
				NazaraInternalError("Invalid pixel format");
				return false;
		}

		NazaraError("Pixel format not handled");

		return false;
	}

	bool CreateTexture(NzTextureImpl* impl, bool proxy)
	{
		OpenGLFormat openGLFormat;
		if (!GetOpenGLFormat(impl->format, &openGLFormat))
		{
			NazaraError("Failed to get OpenGL format");
			return false;
		}

		GLenum target = (proxy) ? openglTargetProxy[impl->type] : openglTarget[impl->type];
		GLint previous;
		glGetIntegerv(openglTargetBinding[impl->type], &previous);
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
				if (glTexStorage2D)
					glTexStorage2D(target, impl->levelCount, openGLFormat.internalFormat, impl->width, impl->height);
				else
				{
					unsigned int size = impl->width; // Les cubemaps ont une longueur et largeur identique
					for (nzUInt8 level = 0; level < impl->levelCount; ++level)
					{
						for (GLenum face : cubemapFace)
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

	static unsigned short lockedLevel[nzImageType_Max+1] = {0};
	static GLuint lockedPrevious[nzImageType_Max+1] = {0};

	void LockTexture(NzTextureImpl* impl)
	{
		if (lockedLevel[impl->type]++ == 0)
		{
			NzContext::EnsureContext();

			GLint previous;
			glGetIntegerv(openglTargetBinding[impl->type], &previous);

			lockedPrevious[impl->type] = static_cast<GLuint>(previous);

			if (lockedPrevious[impl->type] != impl->id)
				glBindTexture(openglTarget[impl->type], impl->id);
		}
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

	void UnlockTexture(NzTextureImpl* impl)
	{
		#ifdef NAZARA_DEBUG
		if (NzContext::GetCurrent() == nullptr)
		{
			NazaraError("No active context");
			return;
		}
		#endif

		#if NAZARA_RENDERER_SAFE
		if (lockedLevel[impl->type] == 0)
		{
			NazaraError("Unlock called on non-locked texture");
			return;
		}
		#endif

		if (--lockedLevel[impl->type] == 0 && lockedPrevious[impl->type] != impl->id)
			glBindTexture(openglTarget[impl->type], lockedPrevious[impl->type]);
	}
}

NzTexture::NzTexture() :
m_impl(nullptr)
{
}

NzTexture::NzTexture(const NzImage& image) :
m_impl(nullptr)
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
}

bool NzTexture::Bind() const
{
	#if NAZARA_RENDERER_SAFE
	if (lockedLevel[m_impl->type] > 0)
	{
		NazaraError("Cannot bind texture while a texture is locked");
		return false;
	}
	#endif

	glBindTexture(openglTarget[m_impl->type], m_impl->id);

	if (m_impl->mipmapping && !m_impl->mipmapsUpdated)
	{
		glGenerateMipmap(openglTarget[m_impl->type]);
		m_impl->mipmapsUpdated = true;
	}

	return true;
}

bool NzTexture::Create(nzImageType type, nzPixelFormat format, unsigned int width, unsigned int height, unsigned int depth, nzUInt8 levelCount, bool lock)
{
	#if NAZARA_RENDERER_SAFE
	if (m_impl && m_impl->isTarget)
	{
		NazaraError("Texture is a target, it cannot be recreated");
		return false;
	}
	#endif

	Destroy();

	if (width == 0 || height == 0 || depth == 0)
		return true;

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

	NzTextureImpl* impl = new NzTextureImpl;
	glGenTextures(1, &impl->id);

	impl->depth = GetValidSize(depth);
	impl->format = format;
	impl->height = GetValidSize(height);
	impl->levelCount = levelCount;
	impl->type = type;
	impl->width = GetValidSize(width);

	LockTexture(impl);

	// Vérification du support par la carte graphique
	/*if (!CreateTexture(impl, true))
	{
		NazaraError("Texture's parameters not supported by driver");
		UnlockTexture(impl);
		glDeleteTextures(1, &impl->id);
		delete impl;

		return false;
	}*/

	// Création de la texture
	if (!CreateTexture(impl, false))
	{
		NazaraError("Failed to create texture");
		UnlockTexture(impl);
		glDeleteTextures(1, &impl->id);
		delete impl;

		return false;
	}

	m_impl = impl;

	// Paramètres par défaut
	SetFilterMode(nzTextureFilter_Nearest);
	SetMipmapRange(0, m_impl->levelCount);
	SetWrapMode(nzTextureWrap_Repeat);

	if (m_impl->levelCount > 1U)
		EnableMipmapping(true);

	if (!lock)
		UnlockTexture(impl);

	return true;
}

void NzTexture::Destroy()
{
	if (m_impl)
	{
		#if NAZARA_RENDERER_SAFE
		if (m_impl->isTarget)
		{
			NazaraError("Texture is a target, it cannot be destroyed");
			return;
		}
		#endif

		NzContext::EnsureContext();

		glDeleteTextures(1, &m_impl->id);
		delete m_impl;
		m_impl = nullptr;
	}
}

bool NzTexture::Download(NzImage* image) const
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
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

	OpenGLFormat format;
	if (!GetOpenGLFormat(m_impl->format, &format))
	{
		NazaraError("Failed to get OpenGL format");
		return false;
	}

	if (!image->Create(m_impl->type, m_impl->format, m_impl->width, m_impl->height, m_impl->depth, m_impl->levelCount))
	{
		NazaraError("Failed to create image");
		return false;
	}

	LockTexture(m_impl);

	unsigned int width = m_impl->width;
	unsigned int height = m_impl->height;
	unsigned int depth = m_impl->depth;

	// Téléchargement...
	for (nzUInt8 level = 0; level < m_impl->levelCount; ++level)
	{
		glGetTexImage(openglTarget[m_impl->type], level, format.dataFormat, format.dataType, image->GetPixels(level));

		if (width > 1)
			width >>= 1;

		if (height > 1)
			height >>= 1;

		if (depth > 1)
			depth >>= 1;
	}

	UnlockTexture(m_impl);

	// Inversion de la texture pour le repère d'OpenGL
	if (!image->FlipVertically())
		NazaraWarning("Failed to flip image");

	return true;
}

bool NzTexture::EnableMipmapping(bool enable)
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return false;
	}
	#endif

	if (m_impl->levelCount == 1)
		return true;

	if (!IsMipmappingSupported())
	{
		NazaraError("Mipmapping not supported");
		return false;
	}

	if (!m_impl->mipmapping && enable)
		m_impl->mipmapsUpdated = false;

	m_impl->mipmapping = enable;

	return true;
}

unsigned int NzTexture::GetAnisotropyLevel() const
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return 0;
	}
	#endif

	if (!NzOpenGL::IsSupported(NzOpenGL::AnisotropicFilter))
		return 1;

	LockTexture(m_impl);

	GLint anisotropyLevel;
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropyLevel);

	UnlockTexture(m_impl);

	return anisotropyLevel;
}

nzUInt8 NzTexture::GetBPP() const
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return 0;
	}
	#endif

	return NzPixelFormat::GetBPP(m_impl->format);
}

unsigned int NzTexture::GetDepth() const
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return 0;
	}
	#endif

	return m_impl->depth;
}

nzTextureFilter NzTexture::GetFilterMode() const
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return nzTextureFilter_Unknown;
	}
	#endif

	LockTexture(m_impl);

	GLint value;
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &value);

	UnlockTexture(m_impl);

	GLenum filterMode = static_cast<GLenum>(value);
	switch (filterMode)
	{
		case GL_LINEAR:
		case GL_LINEAR_MIPMAP_NEAREST:
			return nzTextureFilter_Bilinear;

		case GL_NEAREST:
		case GL_NEAREST_MIPMAP_NEAREST:
			return nzTextureFilter_Nearest;

		case GL_LINEAR_MIPMAP_LINEAR:
			return nzTextureFilter_Trilinear;

		default:
			NazaraInternalError("OpenGL filter mode not handled (0x" + NzString::Number(filterMode, 16) + ')');
			return nzTextureFilter_Unknown;
	}
}

nzPixelFormat NzTexture::GetFormat() const
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
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
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return 0;
	}
	#endif

	return m_impl->height;
}

nzImageType NzTexture::GetType() const
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
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
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return 0;
	}
	#endif

	return m_impl->width;
}

nzTextureWrap NzTexture::GetWrapMode() const
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return nzTextureWrap_Unknown;
	}
	#endif

	LockTexture(m_impl);

	GLint value;
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &value);

	UnlockTexture(m_impl);

	GLenum wrapMode = static_cast<GLenum>(value);
	switch (wrapMode)
	{
		case GL_CLAMP_TO_EDGE:
			return nzTextureWrap_Clamp;

		case GL_REPEAT:
			return nzTextureWrap_Repeat;

		default:
			NazaraInternalError("OpenGL wrap mode not handled (0x" + NzString::Number(wrapMode, 16) + ')');
			return nzTextureWrap_Unknown;
	}
}

bool NzTexture::IsCompressed() const
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
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
	if (!IsValid())
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
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return false;
	}
	#endif

	return m_impl->isTarget;
}

bool NzTexture::IsValid() const
{
	return m_impl != nullptr;
}

bool NzTexture::LoadFromFile(const NzString& filePath, const NzImageParams& params)
{
	NzImage image;
	if (!image.LoadFromFile(filePath, params))
	{
		NazaraError("Failed to load image");
		return false;
	}

	return LoadFromImage(image);
}

bool NzTexture::LoadFromImage(const NzImage& image)
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
		NazaraWarning("Format not supported, trying to convert it to " + NzPixelFormat::ToString(newFormat) + "...");

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
	if (!Create(type, format, newImage.GetWidth(), newImage.GetHeight(), newImage.GetDepth(), newImage.GetLevelCount(), true))
	{
		NazaraError("Failed to create texture");
		return false;
	}

	for (nzUInt8 level = 0; level < m_impl->levelCount; ++level)
	{
		if (!Update(newImage.GetConstPixels(level), level))
		{
			NazaraError("Failed to update texture");
			Destroy();

			return false;
		}
	}

	UnlockTexture(m_impl);

	return true;
}

bool NzTexture::LoadFromMemory(const void* data, std::size_t size, const NzImageParams& params)
{
	NzImage image;
	if (!image.LoadFromMemory(data, size, params))
	{
		NazaraError("Failed to load image");
		return false;
	}

	return LoadFromImage(image);
}

bool NzTexture::LoadFromStream(NzInputStream& stream, const NzImageParams& params)
{
	NzImage image;
	if (!image.LoadFromStream(stream, params))
	{
		NazaraError("Failed to load image");
		return false;
	}

	return LoadFromImage(image);
}

bool NzTexture::Lock()
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return false;
	}
	#endif

	LockTexture(m_impl);

	return true;
}

bool NzTexture::SetAnisotropyLevel(unsigned int anistropyLevel)
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return false;
	}
	#endif

	if (!NzOpenGL::IsSupported(NzOpenGL::AnisotropicFilter) && anistropyLevel > 1)
	{
		NazaraError("Anisotropic filter not supported");
		return false;
	}

	LockTexture(m_impl);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anistropyLevel);

	UnlockTexture(m_impl);

	return true;
}

bool NzTexture::SetFilterMode(nzTextureFilter filter)
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return false;
	}

	if (filter == nzTextureFilter_Trilinear && m_impl->levelCount == 1)
	{
		NazaraError("Trilinear filter set wihout mipmaps");
		return false;
	}
	#endif

	LockTexture(m_impl);

	GLenum target = openglTarget[m_impl->type];
	switch (filter)
	{
		case nzTextureFilter_Bilinear:
			if (m_impl->mipmapping > 1)
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			else
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;

		case nzTextureFilter_Nearest:
			if (m_impl->mipmapping > 1)
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			else
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;

		case nzTextureFilter_Trilinear:
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;

		default:
			NazaraError("Texture filter not handled (0x" + NzString::Number(filter, 16) + ')');
	}

	UnlockTexture(m_impl);

	return true;
}

bool NzTexture::SetMipmapRange(nzUInt8 minLevel, nzUInt8 maxLevel)
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
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

	LockTexture(m_impl);
	glTexParameteri(openglTarget[m_impl->type], GL_TEXTURE_BASE_LEVEL, minLevel);
	glTexParameteri(openglTarget[m_impl->type], GL_TEXTURE_MAX_LEVEL, std::min(m_impl->levelCount, maxLevel));
	UnlockTexture(m_impl);

	return true;
}

bool NzTexture::SetWrapMode(nzTextureWrap wrap)
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return false;
	}
	#endif

	GLenum wrapMode;
	switch (wrap)
	{
		case nzTextureWrap_Clamp:
			wrapMode = GL_CLAMP_TO_EDGE;
			break;

		case nzTextureWrap_Repeat:
			wrapMode = GL_REPEAT;
			break;

		default:
			NazaraError("Texture wrap mode not handled (0x" + NzString::Number(wrap, 16) + ')');
			return false;
	}

	LockTexture(m_impl);

	GLenum target = openglTarget[m_impl->type];
	switch (m_impl->type)
	{
		// Notez l'absence de "break" ici
		case nzImageType_3D:
			glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapMode);
		case nzImageType_2D:
		case nzImageType_2D_Array:
		case nzImageType_Cubemap:
			glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapMode);
		case nzImageType_1D:
		case nzImageType_1D_Array:
			glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapMode);
			break;

		default:
			break;
	}

	UnlockTexture(m_impl);

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

	return Update(image.GetConstPixels(level), level);
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

	const nzUInt8* pixels = image.GetConstPixels(level, rect.x, rect.y, z);
	if (!pixels)
	{
		NazaraError("Failed to access image's pixels");
		return false;
	}

	glPixelStorei(GL_UNPACK_ROW_LENGTH, image.GetWidth(level));

	bool success = Update(pixels, rect, z, level);

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

	return success;
}

bool NzTexture::Update(const NzImage& image, const NzCubeui& cube, nzUInt8 level)
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

	const nzUInt8* pixels = image.GetConstPixels(level, cube.x, cube.y, cube.z);
	if (!pixels)
	{
		NazaraError("Failed to access image's pixels");
		return false;
	}

	glPixelStorei(GL_UNPACK_ROW_LENGTH, image.GetWidth(level));
	glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, image.GetHeight(level));

	bool success = Update(pixels, cube, level);

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);

	return success;
}

bool NzTexture::Update(const nzUInt8* pixels, nzUInt8 level)
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return false;
	}
	#endif

	if (m_impl->type == nzImageType_3D || m_impl->type == nzImageType_2D_Array)
		return Update(pixels, NzCubeui(0, 0, 0, std::max(m_impl->width >> level, 1U), std::max(m_impl->height >> level, 1U), std::max(m_impl->depth >> level, 1U)), level);
	else
		return Update(pixels, NzRectui(0, 0, std::max(m_impl->width >> level, 1U), std::max(m_impl->height >> level, 1U)), 0, level);
}

bool NzTexture::Update(const nzUInt8* pixels, const NzRectui& rect, unsigned int z, nzUInt8 level)
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return false;
	}

	if (m_impl->isTarget)
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

	if (z >= std::max(m_impl->depth >> level, 1U))
	{
		NazaraError("Z value exceeds depth (" + NzString::Number(z) + " >= (" + NzString::Number(m_impl->depth) + ')');
		return false;
	}

	if (level >= m_impl->levelCount)
	{
		NazaraError("Level out of bounds (" + NzString::Number(level) + " >= " + NzString::Number(m_impl->levelCount) + ')');
		return false;
	}
	#endif

	OpenGLFormat format;
	if (!GetOpenGLFormat(m_impl->format, &format))
	{
		NazaraError("Failed to get OpenGL format");
		return false;
	}

	nzUInt8 bpp = NzPixelFormat::GetBPP(m_impl->format);

	// Inversion de la texture pour le repère d'OpenGL
	NzImage mirrored;
	mirrored.Create(m_impl->type, m_impl->format, rect.width, rect.height);
	mirrored.Update(pixels);

	if (!mirrored.FlipVertically())
		NazaraWarning("Failed to flip image");

	SetUnpackAlignement(bpp);

	LockTexture(m_impl);
	switch (m_impl->type)
	{
		case nzImageType_1D:
			glTexSubImage1D(GL_TEXTURE_1D, level, rect.x, rect.width, format.dataFormat, format.dataType, mirrored.GetConstPixels());
			break;

		case nzImageType_1D_Array:
		case nzImageType_2D:
			glTexSubImage2D(openglTarget[m_impl->type], level, rect.x, height-rect.height-rect.y, rect.width, rect.height, format.dataFormat, format.dataType, mirrored.GetConstPixels());
			break;

		case nzImageType_2D_Array:
		case nzImageType_3D:
			glTexSubImage3D(openglTarget[m_impl->type], level, rect.x, height-rect.height-rect.y, z, rect.width, rect.height, 1, format.dataFormat, format.dataType, mirrored.GetConstPixels());
			break;

		case nzImageType_Cubemap:
			NazaraError("Update used on a cubemap texture, please enable safe mode");
			break;
	}
	UnlockTexture(m_impl);

	return true;
}

bool NzTexture::Update(const nzUInt8* pixels, const NzCubeui& cube, nzUInt8 level)
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return false;
	}

	if (m_impl->isTarget)
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

	if (!cube.IsValid())
	{
		NazaraError("Invalid rectangle");
		return false;
	}
	#endif

	unsigned int height = std::max(m_impl->height >> level, 1U);

	#if NAZARA_RENDERER_SAFE
	if (cube.x+cube.width > std::max(m_impl->width >> level, 1U) ||
	    cube.y+cube.height > height ||
		cube.z+cube.depth > std::max(m_impl->depth >> level, 1U))
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

	OpenGLFormat format;
	if (!GetOpenGLFormat(m_impl->format, &format))
	{
		NazaraError("Failed to get OpenGL format");
		return false;
	}

	nzUInt8 bpp = NzPixelFormat::GetBPP(m_impl->format);

	// Inversion de la texture pour le repère d'OpenGL
	unsigned int size = cube.width*cube.height*cube.depth*bpp;
	nzUInt8* mirrored = new nzUInt8[size];
	if (!NzPixelFormat::Flip(nzPixelFlipping_Vertically, m_impl->format, cube.width, cube.height, cube.depth, pixels, mirrored))
	{
		NazaraWarning("Failed to flip image");
		std::memcpy(mirrored, pixels, size);
	}

	SetUnpackAlignement(bpp);

	LockTexture(m_impl);
	switch (m_impl->type)
	{
		case nzImageType_1D:
			glTexSubImage1D(GL_TEXTURE_1D, level, cube.x, cube.width, format.dataFormat, format.dataType, mirrored);
			break;

		case nzImageType_1D_Array:
		case nzImageType_2D:
			glTexSubImage2D(openglTarget[m_impl->type], level, cube.x, height-cube.height-cube.y, cube.width, cube.height, format.dataFormat, format.dataType, mirrored);
			break;

		case nzImageType_2D_Array:
		case nzImageType_3D:
			glTexSubImage3D(openglTarget[m_impl->type], level, cube.x, height-cube.height-cube.y, cube.z, cube.width, cube.height, cube.depth, format.dataFormat, format.dataType, mirrored);
			break;

		case nzImageType_Cubemap:
			NazaraError("Update used on a cubemap texture, please enable safe mode");
			break;
	}
	UnlockTexture(m_impl);

	delete[] mirrored;

	return true;
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

	return UpdateFace(face, image.GetConstPixels(level), NzRectui(0, 0, image.GetWidth(), image.GetHeight()), level);
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

	glPixelStorei(GL_UNPACK_ROW_LENGTH, image.GetWidth(level));

	bool success = UpdateFace(face, image.GetConstPixels(level), rect, level);

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

	return success;
}

bool NzTexture::UpdateFace(nzCubemapFace face, const nzUInt8* pixels, nzUInt8 level)
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return false;
	}
	#endif

	return UpdateFace(face, pixels, NzRectui(0, 0, m_impl->width, m_impl->height), level);
}

bool NzTexture::UpdateFace(nzCubemapFace face, const nzUInt8* pixels, const NzRectui& rect, nzUInt8 level)
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return false;
	}

	if (m_impl->isTarget)
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

	OpenGLFormat format;
	if (!GetOpenGLFormat(m_impl->format, &format))
	{
		NazaraError("Failed to get OpenGL format");
		return false;
	}

	nzUInt8 bpp = NzPixelFormat::GetBPP(m_impl->format);

	// Inversion de la texture pour le repère d'OpenGL
	unsigned int size = rect.width*rect.height*bpp;
	nzUInt8* mirrored = new nzUInt8[size];
	if (!NzPixelFormat::Flip(nzPixelFlipping_Vertically, m_impl->format, rect.width, rect.height, 1, pixels, mirrored))
	{
		NazaraWarning("Failed to flip image");
		std::memcpy(mirrored, pixels, size);
	}

	SetUnpackAlignement(bpp);

	LockTexture(m_impl);
	glTexSubImage2D(cubemapFace[face], level, rect.x, height-rect.height-rect.y, rect.width, rect.height, format.dataFormat, format.dataType, mirrored);
	UnlockTexture(m_impl);


	return true;
}

void NzTexture::Unlock()
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
	{
		NazaraError("Texture must be valid");
		return;
	}
	#endif

	UnlockTexture(m_impl);
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

		// Dépréciés depuis OpenGL 3 (FIXME: Il doit bien exister des remplaçants ..)
		case nzPixelFormat_L8:
		case nzPixelFormat_LA8:
			return false;

		case nzPixelFormat_DXT1:
		case nzPixelFormat_DXT3:
		case nzPixelFormat_DXT5:
		{
			static const bool supported = NzOpenGL::IsSupported(NzOpenGL::TextureCompression_s3tc);
			return supported;
		}

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
		{
			static bool supported = NzOpenGL::IsSupported(NzOpenGL::TextureArray);
			return supported;
		}
	}

	NazaraError("Image type not handled (0x" + NzString::Number(type, 16) + ')');
	return false;
}

void NzTexture::SetTarget(bool isTarget)
{
	#if NAZARA_RENDERER_SAFE
	if (!IsValid())
	{
		NazaraInternalError("Texture must be valid");
		return;
	}
	#endif

	m_impl->isTarget = isTarget;
}
