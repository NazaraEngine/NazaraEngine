// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_IMAGE_HPP
#define NAZARA_IMAGE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/ResourceSaver.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Utility/AbstractImage.hpp>
#include <Nazara/Utility/CubemapParams.hpp>
#include <atomic>

///TODO: Filtres

namespace Nz
{
	struct NAZARA_UTILITY_API ImageParams : ResourceParameters
	{
		// Le format dans lequel l'image doit être chargée (Undefined pour le format le plus proche de l'original)
		PixelFormat loadFormat = PixelFormat::Undefined;

		// Le nombre de niveaux de mipmaps maximum devant être créé
		UInt8 levelCount = 0;

		bool IsValid() const;
	};

	class Image;

	using ImageLibrary = ObjectLibrary<Image>;
	using ImageLoader = ResourceLoader<Image, ImageParams>;
	using ImageManager = ResourceManager<Image, ImageParams>;
	using ImageSaver = ResourceSaver<Image, ImageParams>;

	class NAZARA_UTILITY_API Image : public AbstractImage, public Resource
	{
		public:
			struct SharedImage;

			Image();
			Image(ImageType type, PixelFormat format, unsigned int width, unsigned int height, unsigned int depth = 1, UInt8 levelCount = 1);
			Image(SharedImage* sharedImage);
			Image(const Image& image);
			inline Image(Image&& image) noexcept;
			~Image();

			bool Convert(PixelFormat format);

			void Copy(const Image& source, const Boxui& srcBox, const Vector3ui& dstPos);

			bool Create(ImageType type, PixelFormat format, unsigned int width, unsigned int height, unsigned int depth = 1, UInt8 levelCount = 1);
			void Destroy();

			bool Fill(const Color& color);
			bool Fill(const Color& color, const Boxui& box);
			bool Fill(const Color& color, const Rectui& rect, unsigned int z = 0);

			bool FlipHorizontally();
			bool FlipVertically();

			const UInt8* GetConstPixels(unsigned int x = 0, unsigned int y = 0, unsigned int z = 0, UInt8 level = 0) const;
			unsigned int GetDepth(UInt8 level = 0) const override;
			PixelFormat GetFormat() const override;
			unsigned int GetHeight(UInt8 level = 0) const override;
			UInt8 GetLevelCount() const override;
			UInt8 GetMaxLevel() const override;
			std::size_t GetMemoryUsage() const override;
			std::size_t GetMemoryUsage(UInt8 level) const override;
			Color GetPixelColor(unsigned int x, unsigned int y = 0, unsigned int z = 0) const;
			UInt8* GetPixels(unsigned int x = 0, unsigned int y = 0, unsigned int z = 0, UInt8 level = 0);
			Vector3ui GetSize(UInt8 level = 0) const override;
			ImageType GetType() const override;
			unsigned int GetWidth(UInt8 level = 0) const override;

			bool HasAlpha() const;

			bool IsValid() const;

			// LoadFace
			bool LoadFaceFromFile(CubemapFace face, const std::filesystem::path& filePath, const ImageParams& params = ImageParams());
			bool LoadFaceFromMemory(CubemapFace face, const void* data, std::size_t size, const ImageParams& params = ImageParams());
			bool LoadFaceFromStream(CubemapFace face, Stream& stream, const ImageParams& params = ImageParams());

			// Save
			bool SaveToFile(const std::filesystem::path& filePath, const ImageParams& params = ImageParams());
			bool SaveToStream(Stream& stream, const std::string& format, const ImageParams& params = ImageParams());

			//TODO: SaveArray, SaveCubemap, SaveFace

			void SetLevelCount(UInt8 levelCount);
			bool SetPixelColor(const Color& color, unsigned int x, unsigned int y = 0, unsigned int z = 0);

			bool Update(const UInt8* pixels, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0) override;
			bool Update(const UInt8* pixels, const Boxui& box, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0) override;
			bool Update(const UInt8* pixels, const Rectui& rect, unsigned int z = 0, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0) override;

			Image& operator=(const Image& image);
			inline Image& operator=(Image&& image) noexcept;

			static void Copy(UInt8* destination, const UInt8* source, PixelFormat format, unsigned int width, unsigned int height, unsigned int depth = 1, unsigned int dstWidth = 0, unsigned int dstHeight = 0, unsigned int srcWidth = 0, unsigned int srcHeight = 0);
			static UInt8 GetMaxLevel(unsigned int width, unsigned int height, unsigned int depth = 1);
			static UInt8 GetMaxLevel(ImageType type, unsigned int width, unsigned int height, unsigned int depth = 1);

			// Load
			static std::shared_ptr<Image> LoadFromFile(const std::filesystem::path& filePath, const ImageParams& params = ImageParams());
			static std::shared_ptr<Image> LoadFromMemory(const void* data, std::size_t size, const ImageParams& params = ImageParams());
			static std::shared_ptr<Image> LoadFromStream(Stream& stream, const ImageParams& params = ImageParams());

			// LoadArray
			static std::shared_ptr<Image> LoadArrayFromFile(const std::filesystem::path& filePath, const ImageParams& imageParams = ImageParams(), const Vector2ui& atlasSize = Vector2ui(2, 2));
			static std::shared_ptr<Image> LoadArrayFromImage(const Image& image, const Vector2ui& atlasSize = Vector2ui(2, 2));
			static std::shared_ptr<Image> LoadArrayFromMemory(const void* data, std::size_t size, const ImageParams& imageParams = ImageParams(), const Vector2ui& atlasSize = Vector2ui(2, 2));
			static std::shared_ptr<Image> LoadArrayFromStream(Stream& stream, const ImageParams& imageParams = ImageParams(), const Vector2ui& atlasSize = Vector2ui(2, 2));

			// LoadCubemap
			static std::shared_ptr<Image> LoadCubemapFromFile(const std::filesystem::path& filePath, const ImageParams& imageParams = ImageParams(), const CubemapParams& cubemapParams = CubemapParams());
			static std::shared_ptr<Image> LoadCubemapFromImage(const Image& image, const CubemapParams& params = CubemapParams());
			static std::shared_ptr<Image> LoadCubemapFromMemory(const void* data, std::size_t size, const ImageParams& imageParams = ImageParams(), const CubemapParams& cubemapParams = CubemapParams());
			static std::shared_ptr<Image> LoadCubemapFromStream(Stream& stream, const ImageParams& imageParams = ImageParams(), const CubemapParams& cubemapParams = CubemapParams());

			struct SharedImage
			{
				using PixelContainer = std::vector<std::unique_ptr<UInt8[]>>;

				SharedImage(unsigned short RefCount, ImageType Type, PixelFormat Format, PixelContainer&& Levels, unsigned int Width, unsigned int Height, unsigned int Depth) :
				type(Type),
				format(Format),
				levels(std::move(Levels)),
				depth(Depth),
				height(Height),
				width(Width),
				refCount(RefCount)
				{
				}

				ImageType type;
				PixelFormat format;
				PixelContainer levels;
				unsigned int depth;
				unsigned int height;
				unsigned int width;

				std::atomic_ushort refCount;
			};

			static SharedImage emptyImage;

		private:
			void EnsureOwnership();
			void ReleaseImage();

			SharedImage* m_sharedImage;
		};
}

#include <Nazara/Utility/Image.inl>

#endif // NAZARA_IMAGE_HPP
