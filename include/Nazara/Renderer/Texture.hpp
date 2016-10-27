// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TEXTURE_HPP
#define NAZARA_TEXTURE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Utility/AbstractImage.hpp>
#include <Nazara/Utility/CubemapParams.hpp>
#include <Nazara/Utility/Image.hpp>

namespace Nz
{
	class Texture;

	using TextureConstRef = ObjectRef<const Texture>;
	using TextureLibrary = ObjectLibrary<Texture>;
	using TextureManager = ResourceManager<Texture, ImageParams>;
	using TextureRef = ObjectRef<Texture>;

	struct TextureImpl;

	class NAZARA_RENDERER_API Texture : public AbstractImage, public Resource
	{
		friend TextureLibrary;
		friend TextureManager;
		friend class Renderer;

		public:
			Texture() = default;
			Texture(ImageType type, PixelFormatType format, unsigned int width, unsigned int height, unsigned int depth = 1, UInt8 levelCount = 1);
			explicit Texture(const Image& image);
			Texture(const Texture&) = delete;
			Texture(Texture&&) = delete;
			~Texture();

			bool Create(ImageType type, PixelFormatType format, unsigned int width, unsigned int height, unsigned int depth = 1, UInt8 levelCount = 1);
			void Destroy();

			bool Download(Image* image) const;

			bool EnableMipmapping(bool enable);

			void EnsureMipmapsUpdate() const;

			unsigned int GetDepth(UInt8 level = 0) const override;
			PixelFormatType GetFormat() const override;
			unsigned int GetHeight(UInt8 level = 0) const override;
			UInt8 GetLevelCount() const override;
			UInt8 GetMaxLevel() const override;
			std::size_t GetMemoryUsage() const override;
			std::size_t GetMemoryUsage(UInt8 level) const override;
			Vector3ui GetSize(UInt8 level = 0) const override;
			ImageType GetType() const override;
			unsigned int GetWidth(UInt8 level = 0) const override;

			bool HasMipmaps() const;

			void InvalidateMipmaps();
			bool IsValid() const;

			// Load
			bool LoadFromFile(const String& filePath, const ImageParams& params = ImageParams(), bool generateMipmaps = true);
			bool LoadFromImage(const Image& image, bool generateMipmaps = true);
			bool LoadFromMemory(const void* data, std::size_t size, const ImageParams& params = ImageParams(), bool generateMipmaps = true);
			bool LoadFromStream(Stream& stream, const ImageParams& params = ImageParams(), bool generateMipmaps = true);

			// LoadArray
			bool LoadArrayFromFile(const String& filePath, const ImageParams& imageParams = ImageParams(), bool generateMipmaps = true, const Vector2ui& atlasSize = Vector2ui(2, 2));
			bool LoadArrayFromImage(const Image& image, bool generateMipmaps = true, const Vector2ui& atlasSize = Vector2ui(2, 2));
			bool LoadArrayFromMemory(const void* data, std::size_t size, const ImageParams& imageParams = ImageParams(), bool generateMipmaps = true, const Vector2ui& atlasSize = Vector2ui(2, 2));
			bool LoadArrayFromStream(Stream& stream, const ImageParams& imageParams = ImageParams(), bool generateMipmaps = true, const Vector2ui& atlasSize = Vector2ui(2, 2));

			// LoadCubemap
			bool LoadCubemapFromFile(const String& filePath, const ImageParams& imageParams = ImageParams(), bool generateMipmaps = true, const CubemapParams& cubemapParams = CubemapParams());
			bool LoadCubemapFromImage(const Image& image, bool generateMipmaps = true, const CubemapParams& params = CubemapParams());
			bool LoadCubemapFromMemory(const void* data, std::size_t size, const ImageParams& imageParams = ImageParams(), bool generateMipmaps = true, const CubemapParams& cubemapParams = CubemapParams());
			bool LoadCubemapFromStream(Stream& stream, const ImageParams& imageParams = ImageParams(), bool generateMipmaps = true, const CubemapParams& cubemapParams = CubemapParams());

			// LoadFace
			bool LoadFaceFromFile(CubemapFace face, const String& filePath, const ImageParams& params = ImageParams());
			bool LoadFaceFromMemory(CubemapFace face, const void* data, std::size_t size, const ImageParams& params = ImageParams());
			bool LoadFaceFromStream(CubemapFace face, Stream& stream, const ImageParams& params = ImageParams());

			// Save
			bool SaveToFile(const String& filePath, const ImageParams& params = ImageParams());
			bool SaveToStream(Stream& stream, const String& format, const ImageParams& params = ImageParams());

			bool SetMipmapRange(UInt8 minLevel, UInt8 maxLevel);

			bool Update(const Image& image, UInt8 level = 0);
			bool Update(const Image& image, const Boxui& box, UInt8 level = 0);
			bool Update(const Image& image, const Rectui& rect, unsigned int z = 0, UInt8 level = 0);
			bool Update(const UInt8* pixels, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0) override;
			bool Update(const UInt8* pixels, const Boxui& box, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0) override;
			bool Update(const UInt8* pixels, const Rectui& rect, unsigned int z = 0, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0) override;

			// Fonctions OpenGL
			unsigned int GetOpenGLID() const;

			Texture& operator=(const Texture&) = delete;
			Texture& operator=(Texture&&) = delete;

			static bool IsFormatSupported(PixelFormatType format);
			static bool IsMipmappingSupported();
			static bool IsTypeSupported(ImageType type);
			template<typename... Args> static TextureRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnTextureDestroy, const Texture* /*texture*/);
			NazaraSignal(OnTextureRelease, const Texture* /*texture*/);

		private:
			bool CreateTexture(bool proxy);

			static bool Initialize();
			static void Uninitialize();

			TextureImpl* m_impl = nullptr;

			static TextureLibrary::LibraryMap s_library;
			static TextureManager::ManagerMap s_managerMap;
			static TextureManager::ManagerParams s_managerParameters;
	};
}

#include <Nazara/Renderer/Texture.inl>

#endif // NAZARA_TEXTURE_HPP
