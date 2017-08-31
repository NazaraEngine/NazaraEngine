// Copyright (C) 2017 Jérôme Leclercq
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

namespace Nz
{
	struct TextureImpl
	{
		GLuint id;
		ImageType type;
		PixelFormatType format;
		UInt8 levelCount;
		bool mipmapping = false;
		bool mipmapsUpdated = true;
		unsigned int depth;
		unsigned int height;
		unsigned int width;
	};

	namespace
	{
		inline unsigned int GetLevelSize(unsigned int size, UInt8 level)
		{
			// Contrairement à la classe Image, un appel à GetLevelSize(0, level) n'est pas possible
			return std::max(size >> level, 1U);
		}

		inline void SetUnpackAlignement(UInt8 bpp)
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

	Texture::Texture(ImageType type, PixelFormatType format, unsigned int width, unsigned int height, unsigned int depth, UInt8 levelCount)
	{
		ErrorFlags flags(ErrorFlag_ThrowException);
		Create(type, format, width, height, depth, levelCount);
	}

	Texture::Texture(const Image& image)
	{
		ErrorFlags flags(ErrorFlag_ThrowException);
		LoadFromImage(image);
	}

	Texture::~Texture()
	{
		OnTextureRelease(this);

		Destroy();
		Renderer::OnTextureReleased(this); ///TODO: Gets rid of this
	}

	bool Texture::Create(ImageType type, PixelFormatType format, unsigned int width, unsigned int height, unsigned int depth, UInt8 levelCount)
	{
		Destroy();

		#if NAZARA_RENDERER_SAFE
		if (!IsTypeSupported(type))
		{
			NazaraError("Texture's type not supported");
			return false;
		}

		if (!PixelFormat::IsValid(format))
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
			case ImageType_1D:
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
		}
		#endif

		Context::EnsureContext();

		if (IsMipmappingSupported())
			levelCount = std::min(levelCount, Image::GetMaxLevel(width, height, depth));
		else if (levelCount > 1)
		{
			NazaraWarning("Mipmapping not supported, reducing level count to 1");
			levelCount = 1;
		}

		m_impl = new TextureImpl;
		m_impl->depth = depth;
		m_impl->format = format;
		m_impl->height = height;
		m_impl->levelCount = levelCount;
		m_impl->type = type;
		m_impl->width = width;

		glGenTextures(1, &m_impl->id);
		OpenGL::BindTexture(m_impl->type, m_impl->id);

		// En cas d'erreur (sortie prématurée), on détruit la texture
		CallOnExit onExit([this]()
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
		return true;
	}

	void Texture::Destroy()
	{
		if (m_impl)
		{
			OnTextureDestroy(this);

			Context::EnsureContext();
			OpenGL::DeleteTexture(m_impl->id);

			delete m_impl;
			m_impl = nullptr;
		}
	}

	bool Texture::Download(Image* image) const
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

		OpenGL::Format format;
		if (!OpenGL::TranslateFormat(m_impl->format, &format, OpenGL::FormatType_Texture))
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
		OpenGL::BindTexture(m_impl->type, m_impl->id);
		for (UInt8 level = 0; level < m_impl->levelCount; ++level)
			glGetTexImage(OpenGL::TextureTarget[m_impl->type], level, format.dataFormat, format.dataType, image->GetPixels(0, 0, 0, level));

		return true;
	}

	bool Texture::EnableMipmapping(bool enable)
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
			m_impl->levelCount = Image::GetMaxLevel(m_impl->width, m_impl->height, m_impl->depth);
			SetMipmapRange(0, m_impl->levelCount-1);
		}

		if (!m_impl->mipmapping && enable)
			m_impl->mipmapsUpdated = false;

		m_impl->mipmapping = enable;

		return true;
	}

	void Texture::EnsureMipmapsUpdate() const
	{
		if (m_impl->mipmapping && !m_impl->mipmapsUpdated)
		{
			OpenGL::BindTexture(m_impl->type, m_impl->id);
			glGenerateMipmap(OpenGL::TextureTarget[m_impl->type]);
			m_impl->mipmapsUpdated = true;
		}
	}

	unsigned int Texture::GetDepth(UInt8 level) const
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

	PixelFormatType Texture::GetFormat() const
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_impl)
		{
			NazaraError("Texture must be valid");
			return PixelFormatType_Undefined;
		}
		#endif

		return m_impl->format;
	}

	unsigned int Texture::GetHeight(UInt8 level) const
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

	UInt8 Texture::GetLevelCount() const
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

	UInt8 Texture::GetMaxLevel() const
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_impl)
		{
			NazaraError("Texture must be valid");
			return 0;
		}
		#endif

		return Image::GetMaxLevel(m_impl->type, m_impl->width, m_impl->height, m_impl->depth);
	}

	std::size_t Texture::GetMemoryUsage() const
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

		if (m_impl->type == ImageType_Cubemap)
			size *= 6;

		return size * PixelFormat::GetBytesPerPixel(m_impl->format);
	}

	std::size_t Texture::GetMemoryUsage(UInt8 level) const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Texture must be valid");
			return 0;
		}

		if (level >= m_impl->levelCount)
		{
			NazaraError("Level out of bounds (" + String::Number(level) + " >= " + String::Number(m_impl->levelCount) + ')');
			return 0;
		}
		#endif

		return (GetLevelSize(m_impl->width, level)) *
			   (GetLevelSize(m_impl->height, level)) *
			   ((m_impl->type == ImageType_Cubemap) ? 6 : GetLevelSize(m_impl->depth, level)) *
			   PixelFormat::GetBytesPerPixel(m_impl->format);
	}

	Vector3ui Texture::GetSize(UInt8 level) const
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_impl)
		{
			NazaraError("Texture must be valid");
			return Vector3ui(0, 0, 0);
		}
		#endif

		return Vector3ui(GetLevelSize(m_impl->width, level), GetLevelSize(m_impl->height, level), GetLevelSize(m_impl->depth, level));
	}

	ImageType Texture::GetType() const
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_impl)
		{
			NazaraError("Texture must be valid");
			return ImageType_2D;
		}
		#endif

		return m_impl->type;
	}

	unsigned int Texture::GetWidth(UInt8 level) const
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

	bool Texture::HasMipmaps() const
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

	void Texture::InvalidateMipmaps()
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

	bool Texture::IsValid() const
	{
		return m_impl != nullptr;
	}

	bool Texture::LoadFromFile(const String& filePath, const ImageParams& params, bool generateMipmaps)
	{
		Image image;
		if (!image.LoadFromFile(filePath, params))
		{
			NazaraError("Failed to load image");
			return false;
		}

		return LoadFromImage(image, generateMipmaps);
	}

	bool Texture::LoadFromImage(const Image& image, bool generateMipmaps)
	{
		#if NAZARA_RENDERER_SAFE
		if (!image.IsValid())
		{
			NazaraError("Image must be valid");
			return false;
		}
		#endif

		// Vive le Copy-On-Write
		Image newImage(image);

		PixelFormatType format = newImage.GetFormat();
		if (!IsFormatSupported(format))
		{
			///TODO: Sélectionner le format le plus adapté selon les composantes présentes dans le premier format
			PixelFormatType newFormat = (PixelFormat::HasAlpha(format)) ? PixelFormatType_BGRA8 : PixelFormatType_BGR8;
			NazaraWarning("Format " + PixelFormat::GetName(format) + " not supported, trying to convert it to " + PixelFormat::GetName(newFormat) + "...");

			if (PixelFormat::IsConversionSupported(format, newFormat))
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

		ImageType type = newImage.GetType();
		UInt8 levelCount = newImage.GetLevelCount();
		if (!Create(type, format, newImage.GetWidth(), newImage.GetHeight(), newImage.GetDepth(), (generateMipmaps) ? 0xFF : levelCount))
		{
			NazaraError("Failed to create texture");
			return false;
		}

		CallOnExit destroyOnExit([this]()
		{
			Destroy();
		});

		if (type == ImageType_Cubemap)
		{
			for (UInt8 level = 0; level < levelCount; ++level)
			{
				for (unsigned int i = 0; i <= CubemapFace_Max; ++i)
				{
					if (!Update(newImage.GetConstPixels(0, 0, i, level), Rectui(0, 0, newImage.GetWidth(level), newImage.GetHeight(level)), i, level))
					{
						NazaraError("Failed to update texture");
						return false;
					}
				}
			}
		}
		else
		{
			for (UInt8 level = 0; level < levelCount; ++level)
			{
				if (!Update(newImage.GetConstPixels(0, 0, 0, level), level))
				{
					NazaraError("Failed to update texture");
					return false;
				}
			}
		}

		// Keep resource path info
		SetFilePath(image.GetFilePath());

		destroyOnExit.Reset();

		return true;
	}

	bool Texture::LoadFromMemory(const void* data, std::size_t size, const ImageParams& params, bool generateMipmaps)
	{
		Image image;
		if (!image.LoadFromMemory(data, size, params))
		{
			NazaraError("Failed to load image");
			return false;
		}

		return LoadFromImage(image, generateMipmaps);
	}

	bool Texture::LoadFromStream(Stream& stream, const ImageParams& params, bool generateMipmaps)
	{
		Image image;
		if (!image.LoadFromStream(stream, params))
		{
			NazaraError("Failed to load image");
			return false;
		}

		return LoadFromImage(image, generateMipmaps);
	}

	bool Texture::LoadArrayFromFile(const String& filePath, const ImageParams& imageParams, bool generateMipmaps, const Vector2ui& atlasSize)
	{
		Image cubemap;
		if (!cubemap.LoadArrayFromFile(filePath, imageParams, atlasSize))
		{
			NazaraError("Failed to load cubemap");
			return false;
		}

		return LoadFromImage(cubemap, generateMipmaps);
	}

	bool Texture::LoadArrayFromImage(const Image& image, bool generateMipmaps, const Vector2ui& atlasSize)
	{
		Image cubemap;
		if (!cubemap.LoadArrayFromImage(image, atlasSize))
		{
			NazaraError("Failed to load cubemap");
			return false;
		}

		return LoadFromImage(cubemap, generateMipmaps);
	}

	bool Texture::LoadArrayFromMemory(const void* data, std::size_t size, const ImageParams& imageParams, bool generateMipmaps, const Vector2ui& atlasSize)
	{
		Image cubemap;
		if (!cubemap.LoadArrayFromMemory(data, size, imageParams, atlasSize))
		{
			NazaraError("Failed to load cubemap");
			return false;
		}

		return LoadFromImage(cubemap, generateMipmaps);
	}

	bool Texture::LoadArrayFromStream(Stream& stream, const ImageParams& imageParams, bool generateMipmaps, const Vector2ui& atlasSize)
	{
		Image cubemap;
		if (!cubemap.LoadArrayFromStream(stream, imageParams, atlasSize))
		{
			NazaraError("Failed to load cubemap");
			return false;
		}

		return LoadFromImage(cubemap, generateMipmaps);
	}

	bool Texture::LoadCubemapFromFile(const String& filePath, const ImageParams& imageParams, bool generateMipmaps, const CubemapParams& cubemapParams)
	{
		Image cubemap;
		if (!cubemap.LoadCubemapFromFile(filePath, imageParams, cubemapParams))
		{
			NazaraError("Failed to load cubemap");
			return false;
		}

		return LoadFromImage(cubemap, generateMipmaps);
	}

	bool Texture::LoadCubemapFromImage(const Image& image, bool generateMipmaps, const CubemapParams& params)
	{
		Image cubemap;
		if (!cubemap.LoadCubemapFromImage(image, params))
		{
			NazaraError("Failed to load cubemap");
			return false;
		}

		return LoadFromImage(cubemap, generateMipmaps);
	}

	bool Texture::LoadCubemapFromMemory(const void* data, std::size_t size, const ImageParams& imageParams, bool generateMipmaps, const CubemapParams& cubemapParams)
	{
		Image cubemap;
		if (!cubemap.LoadCubemapFromMemory(data, size, imageParams, cubemapParams))
		{
			NazaraError("Failed to load cubemap");
			return false;
		}

		return LoadFromImage(cubemap, generateMipmaps);
	}

	bool Texture::LoadCubemapFromStream(Stream& stream, const ImageParams& imageParams, bool generateMipmaps, const CubemapParams& cubemapParams)
	{
		Image cubemap;
		if (!cubemap.LoadCubemapFromStream(stream, imageParams, cubemapParams))
		{
			NazaraError("Failed to load cubemap");
			return false;
		}

		return LoadFromImage(cubemap, generateMipmaps);
	}

	bool Texture::LoadFaceFromFile(CubemapFace face, const String& filePath, const ImageParams& params)
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_impl)
		{
			NazaraError("Texture must be valid");
			return false;
		}

		if (m_impl->type != ImageType_Cubemap)
		{
			NazaraError("Texture must be a cubemap");
			return false;
		}
		#endif

		Image image;
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

		return Update(image, Rectui(0, 0, faceSize, faceSize), face);
	}

	bool Texture::LoadFaceFromMemory(CubemapFace face, const void* data, std::size_t size, const ImageParams& params)
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_impl)
		{
			NazaraError("Texture must be valid");
			return false;
		}

		if (m_impl->type != ImageType_Cubemap)
		{
			NazaraError("Texture must be a cubemap");
			return false;
		}
		#endif

		Image image;
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

		return Update(image, Rectui(0, 0, faceSize, faceSize), face);
	}

	bool Texture::LoadFaceFromStream(CubemapFace face, Stream& stream, const ImageParams& params)
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_impl)
		{
			NazaraError("Texture must be valid");
			return false;
		}

		if (m_impl->type != ImageType_Cubemap)
		{
			NazaraError("Texture must be a cubemap");
			return false;
		}
		#endif

		Image image;
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

		return Update(image, Rectui(0, 0, faceSize, faceSize), face);
	}

	bool Texture::SaveToFile(const String& filePath, const ImageParams& params)
	{
		Image image;
		if (!Download(&image))
		{
			NazaraError("Failed to download texture");
			return false;
		}

		return image.SaveToFile(filePath, params);
	}

	bool Texture::SaveToStream(Stream& stream, const String& format, const ImageParams& params)
	{
		Image image;
		if (!Download(&image))
		{
			NazaraError("Failed to download texture");
			return false;
		}

		return image.SaveToStream(stream, format, params);
	}

	bool Texture::SetMipmapRange(UInt8 minLevel, UInt8 maxLevel)
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_impl)
		{
			NazaraError("Texture must be valid");
			return false;
		}

		if (minLevel >= m_impl->levelCount)
		{
			NazaraError("Minimum level cannot be greater or equal than level count (" + String::Number(minLevel) + " >= " + String::Number(m_impl->levelCount) + ')');
			return false;
		}

		if (maxLevel < minLevel)
		{
			NazaraError("Minimum level cannot be greater than maximum level (" + String::Number(minLevel) + " < " + String::Number(maxLevel) + ')');
			return false;
		}
		#endif

		OpenGL::BindTexture(m_impl->type, m_impl->id);
		glTexParameteri(OpenGL::TextureTarget[m_impl->type], GL_TEXTURE_BASE_LEVEL, minLevel);
		glTexParameteri(OpenGL::TextureTarget[m_impl->type], GL_TEXTURE_MAX_LEVEL, maxLevel);

		return true;
	}

	bool Texture::Update(const Image& image, UInt8 level)
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

		const UInt8* pixels = image.GetConstPixels(0, 0, 0, level);
		if (!pixels)
		{
			NazaraError("Failed to access image's pixels");
			return false;
		}

		return Update(pixels, image.GetWidth(level), image.GetHeight(level), level);
	}

	bool Texture::Update(const Image& image, const Boxui& box, UInt8 level)
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

		const UInt8* pixels = image.GetConstPixels(0, 0, 0, level);
		if (!pixels)
		{
			NazaraError("Failed to access image's pixels");
			return false;
		}

		return Update(pixels, box, image.GetWidth(level), image.GetHeight(level), level);
	}

	bool Texture::Update(const Image& image, const Rectui& rect, unsigned int z, UInt8 level)
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

		const UInt8* pixels = image.GetConstPixels(0, 0, 0, level);
		if (!pixels)
		{
			NazaraError("Failed to access image's pixels");
			return false;
		}

		return Update(pixels, rect, z, image.GetWidth(level), image.GetHeight(level), level);
	}

	bool Texture::Update(const UInt8* pixels, unsigned int srcWidth, unsigned int srcHeight, UInt8 level)
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_impl)
		{
			NazaraError("Texture must be valid");
			return false;
		}
		#endif

		return Update(pixels, Boxui(GetLevelSize(m_impl->width, level), GetLevelSize(m_impl->height, level), GetLevelSize(m_impl->depth, level)), srcWidth, srcHeight, level);
	}

	bool Texture::Update(const UInt8* pixels, const Boxui& box, unsigned int srcWidth, unsigned int srcHeight, UInt8 level)
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

		unsigned int width = GetLevelSize(m_impl->width, level);
		unsigned int height = GetLevelSize(m_impl->height, level);
		unsigned int depth = (m_impl->type == ImageType_Cubemap) ? 6 : GetLevelSize(m_impl->depth, level);
		if (box.x+box.width > width || box.y+box.height > height || box.z+box.depth > depth ||
			(m_impl->type == ImageType_Cubemap && box.depth > 1)) // Nous n'autorisons pas de modifier plus d'une face du cubemap à la fois
		{
			NazaraError("Cube dimensions are out of bounds");
			return false;
		}

		if (level >= m_impl->levelCount)
		{
			NazaraError("Level out of bounds (" + String::Number(level) + " >= " + String::Number(m_impl->levelCount) + ')');
			return false;
		}
		#endif

		OpenGL::Format format;
		if (!OpenGL::TranslateFormat(m_impl->format, &format, OpenGL::FormatType_Texture))
		{
			NazaraError("Failed to get OpenGL format");
			return false;
		}

		SetUnpackAlignement(PixelFormat::GetBytesPerPixel(m_impl->format));
		glPixelStorei(GL_UNPACK_ROW_LENGTH, srcWidth);
		glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, srcHeight);

		OpenGL::BindTexture(m_impl->type, m_impl->id);

		if (PixelFormat::IsCompressed(m_impl->format))
		{
			switch (m_impl->type)
			{
				case ImageType_1D:
					glCompressedTexSubImage1D(GL_TEXTURE_1D, level, box.x, box.width, format.internalFormat, PixelFormat::ComputeSize(m_impl->format, box.width, 1, 1), pixels);
					break;

				case ImageType_1D_Array:
				case ImageType_2D:
					glCompressedTexSubImage2D(OpenGL::TextureTarget[m_impl->type], level, box.x, box.y, box.width, box.height, format.internalFormat, PixelFormat::ComputeSize(m_impl->format, box.width, box.height, 1), pixels);
					break;

				case ImageType_2D_Array:
				case ImageType_3D:
					glCompressedTexSubImage3D(OpenGL::TextureTarget[m_impl->type], level, box.x, box.y, box.z, box.width, box.height, box.depth, format.internalFormat, PixelFormat::ComputeSize(m_impl->format, box.width, box.height, box.depth), pixels);
					break;

				case ImageType_Cubemap:
					glCompressedTexSubImage2D(OpenGL::CubemapFace[box.z], level, box.x, box.y, box.width, box.height, format.internalFormat, PixelFormat::ComputeSize(m_impl->format, box.width, box.height, box.depth), pixels);
					break;
			}
		}
		else
		{
			switch (m_impl->type)
			{
				case ImageType_1D:
					glTexSubImage1D(GL_TEXTURE_1D, level, box.x, box.width, format.dataFormat, format.dataType, pixels);
					break;

				case ImageType_1D_Array:
				case ImageType_2D:
					glTexSubImage2D(OpenGL::TextureTarget[m_impl->type], level, box.x, box.y, box.width, box.height, format.dataFormat, format.dataType, pixels);
					break;

				case ImageType_2D_Array:
				case ImageType_3D:
					glTexSubImage3D(OpenGL::TextureTarget[m_impl->type], level, box.x, box.y, box.z, box.width, box.height, box.depth, format.dataFormat, format.dataType, pixels);
					break;

				case ImageType_Cubemap:
					glTexSubImage2D(OpenGL::CubemapFace[box.z], level, box.x, box.y, box.width, box.height, format.dataFormat, format.dataType, pixels);
					break;
			}
		}

		return true;
	}

	bool Texture::Update(const UInt8* pixels, const Rectui& rect, unsigned int z, unsigned int srcWidth, unsigned int srcHeight, UInt8 level)
	{
		return Update(pixels, Boxui(rect.x, rect.y, z, rect.width, rect.height, 1), srcWidth, srcHeight, level);
	}

	unsigned int Texture::GetOpenGLID() const
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

	bool Texture::IsFormatSupported(PixelFormatType format)
	{
		switch (format)
		{
			// Formats de base
			case PixelFormatType_A8:
			case PixelFormatType_BGR8:
			case PixelFormatType_BGRA8:
			case PixelFormatType_L8:
			case PixelFormatType_LA8:
			case PixelFormatType_RGB8:
			case PixelFormatType_RGBA8:
				return true;

			// Packed formats supportés depuis OpenGL 1.2
			case PixelFormatType_RGB5A1:
			case PixelFormatType_RGBA4:
				return true;

			// Formats supportés depuis OpenGL 3
			case PixelFormatType_R8:
			case PixelFormatType_R8I:
			case PixelFormatType_R8UI:
			case PixelFormatType_R16:
			case PixelFormatType_R16F:
			case PixelFormatType_R16I:
			case PixelFormatType_R16UI:
			case PixelFormatType_R32F:
			case PixelFormatType_R32I:
			case PixelFormatType_R32UI:
			case PixelFormatType_RG8:
			case PixelFormatType_RG8I:
			case PixelFormatType_RG8UI:
			case PixelFormatType_RG16:
			case PixelFormatType_RG16F:
			case PixelFormatType_RG16I:
			case PixelFormatType_RG16UI:
			case PixelFormatType_RG32F:
			case PixelFormatType_RG32I:
			case PixelFormatType_RG32UI:
			case PixelFormatType_RGB16F:
			case PixelFormatType_RGB16I:
			case PixelFormatType_RGB16UI:
			case PixelFormatType_RGB32F:
			case PixelFormatType_RGB32I:
			case PixelFormatType_RGB32UI:
			case PixelFormatType_RGBA16F:
			case PixelFormatType_RGBA16I:
			case PixelFormatType_RGBA16UI:
			case PixelFormatType_RGBA32F:
			case PixelFormatType_RGBA32I:
			case PixelFormatType_RGBA32UI:
				return true;

			// Formats de profondeur (Supportés avec les FBOs)
			case PixelFormatType_Depth16:
			case PixelFormatType_Depth24:
			case PixelFormatType_Depth32:
			case PixelFormatType_Depth24Stencil8:
				return true;

			// Formats de stencil (Non supportés pour les textures)
			case PixelFormatType_Stencil1:
			case PixelFormatType_Stencil4:
			case PixelFormatType_Stencil8:
			case PixelFormatType_Stencil16:
				return false;

			// Formats compressés
			case PixelFormatType_DXT1:
			case PixelFormatType_DXT3:
			case PixelFormatType_DXT5:
				return OpenGL::IsSupported(OpenGLExtension_TextureCompression_s3tc);

			case PixelFormatType_Undefined:
				break;
		}

		NazaraError("Invalid pixel format");
		return false;
	}

	bool Texture::IsMipmappingSupported()
	{
		return glGenerateMipmap != nullptr;
	}

	bool Texture::IsTypeSupported(ImageType type)
	{
		switch (type)
		{
			case ImageType_1D:
			case ImageType_1D_Array:
			case ImageType_2D:
			case ImageType_2D_Array:
			case ImageType_3D:
			case ImageType_Cubemap:
				return true; // Tous supportés nativement dans OpenGL 3
		}

		NazaraError("Image type not handled (0x" + String::Number(type, 16) + ')');
		return false;
	}

	bool Texture::CreateTexture(bool proxy)
	{
		OpenGL::Format openGLFormat;
		if (!OpenGL::TranslateFormat(m_impl->format, &openGLFormat, OpenGL::FormatType_Texture))
		{
			NazaraError("Format " + PixelFormat::GetName(m_impl->format) + " not supported by OpenGL");
			return false;
		}

		GLenum target = (proxy) ? OpenGL::TextureTargetProxy[m_impl->type] : OpenGL::TextureTarget[m_impl->type];
		switch (m_impl->type)
		{
			case ImageType_1D:
			{
				if (glTexStorage1D && !proxy) // Les drivers AMD semblent ne pas aimer glTexStorage avec un format proxy
					glTexStorage1D(target, m_impl->levelCount, openGLFormat.internalFormat, m_impl->width);
				else
				{
					unsigned int w = m_impl->width;
					for (UInt8 level = 0; level < m_impl->levelCount; ++level)
					{
						glTexImage1D(target, level, openGLFormat.internalFormat, w, 0, openGLFormat.dataFormat, openGLFormat.dataType, nullptr);
						if (w > 1U)
							w >>= 1;
					}
				}
				break;
			}

			case ImageType_1D_Array:
			case ImageType_2D:
			{
				if (glTexStorage2D && !proxy)
					glTexStorage2D(target, m_impl->levelCount, openGLFormat.internalFormat, m_impl->width, m_impl->height);
				else
				{
					unsigned int w = m_impl->width;
					unsigned int h = m_impl->height;
					for (UInt8 level = 0; level < m_impl->levelCount; ++level)
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

			case ImageType_2D_Array:
			case ImageType_3D:
			{
				if (glTexStorage3D && !proxy)
					glTexStorage3D(target, m_impl->levelCount, openGLFormat.internalFormat, m_impl->width, m_impl->height, m_impl->depth);
				else
				{
					unsigned int w = m_impl->width;
					unsigned int h = m_impl->height;
					unsigned int d = m_impl->depth;
					for (UInt8 level = 0; level < m_impl->levelCount; ++level)
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

			case ImageType_Cubemap:
			{
				if (glTexStorage2D && !proxy)
					glTexStorage2D(target, m_impl->levelCount, openGLFormat.internalFormat, m_impl->width, m_impl->height);
				else
				{
					unsigned int size = m_impl->width; // Les cubemaps ont une longueur et largeur identique
					for (UInt8 level = 0; level < m_impl->levelCount; ++level)
					{
						for (GLenum face : OpenGL::CubemapFace)
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
		if (!proxy && OpenGL::GetVersion() >= 300)
		{
			glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, openGLFormat.swizzle[0]);
			glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, openGLFormat.swizzle[1]);
			glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, openGLFormat.swizzle[2]);
			glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, openGLFormat.swizzle[3]);
		}

		if (!proxy && PixelFormat::GetContent(m_impl->format) == PixelFormatContent_DepthStencil)
		{
			glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTexParameteri(target, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		}

		return true;
	}

	bool Texture::Initialize()
	{
		if (!TextureLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		if (!TextureManager::Initialize())
		{
			NazaraError("Failed to initialise manager");
			return false;
		}

		return true;
	}

	void Texture::Uninitialize()
	{
		TextureManager::Uninitialize();
		TextureLibrary::Uninitialize();
	}

	TextureLibrary::LibraryMap Texture::s_library;
	TextureManager::ManagerMap Texture::s_managerMap;
	TextureManager::ManagerParams Texture::s_managerParameters;
}
