// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_IMAGE_HPP
#define NAZARA_CORE_IMAGE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/AbstractImage.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/CubemapParams.hpp>
#include <Nazara/Core/ImageUtils.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/ResourceSaver.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <NazaraUtils/Signal.hpp>
#include <atomic>
#include <bitset>

///TODO: Filtres

namespace Nz
{
	struct NAZARA_CORE_API ImageParams : ResourceParameters
	{
		// Which pixel format should the image be loaded in (Undefined for the closest format available)
		PixelFormat loadFormat = PixelFormat::Undefined;

		// Maximum mimaps level that should be created / loaded (0 for max)
		UInt8 levelCount = 0;

		// Which levels to load/save
		std::bitset<ImageUtils::MaxLevels> levels = 0xFFFFFFFF;

		bool IsValid() const;
		void Merge(const ImageParams& params);
	};

	class Image;

	using ImageLibrary = ObjectLibrary<Image>;
	using ImageLoader = ResourceLoader<Image, ImageParams>;
	using ImageManager = ResourceManager<Image, ImageParams>;
	using ImageSaver = ResourceSaver<Image, ImageParams>;

	class NAZARA_CORE_API Image : public AbstractImage, public Resource
	{
		public:
			using Params = ImageParams;
			struct SharedImage;

			Image();
			Image(ImageType type, PixelFormat format, UInt32 width, UInt32 height, UInt32 depth = 1, UInt8 levelCount = 1);
			Image(SharedImage* sharedImage);
			Image(const Image& image);
			inline Image(Image&& image) noexcept;
			~Image();

			bool Convert(PixelFormat format);

			void Copy(const Image& source, const Boxui32& srcBox, const Vector3ui32& dstPos);

			bool Create(ImageType type, PixelFormat format, UInt32 width, UInt32 height, UInt32 depth = 1, UInt8 levelCount = 1);
			void Destroy();

			bool Fill(const Color& color);
			bool Fill(const Color& color, const Boxui32& box);
			bool Fill(const Color& color, const Rectui32& rect, UInt32 z = 0);

			bool FlipHorizontally();
			bool FlipVertically();

			void FreeLevel(UInt8 level);

			const UInt8* GetConstPixels(UInt32 x = 0, UInt32 y = 0, UInt32 z = 0, UInt8 level = 0) const;
			UInt32 GetDepth(UInt8 level = 0) const;
			PixelFormat GetFormat() const override;
			UInt32 GetHeight(UInt8 level = 0) const;
			UInt8 GetLevelCount() const override;
			UInt8 GetMaxLevel() const;
			std::size_t GetMemoryUsage() const;
			std::size_t GetMemoryUsage(UInt8 level) const;
			Color GetPixelColor(UInt32 x, UInt32 y = 0, UInt32 z = 0) const;
			UInt8* GetPixels(UInt32 x = 0, UInt32 y = 0, UInt32 z = 0, UInt8 level = 0);
			Vector3ui32 GetSize(UInt8 level = 0) const override;
			ImageType GetType() const override;
			UInt32 GetWidth(UInt8 level = 0) const;

			bool HasAlpha() const;

			bool IsValid() const;

			// LoadFace
			bool LoadFaceFromFile(CubemapFace face, const std::filesystem::path& filePath, const ImageParams& params = ImageParams());
			bool LoadFaceFromImage(CubemapFace face, const Image& image);
			bool LoadFaceFromMemory(CubemapFace face, const void* data, std::size_t size, const ImageParams& params = ImageParams());
			bool LoadFaceFromStream(CubemapFace face, Stream& stream, const ImageParams& params = ImageParams());

			// Save
			bool SaveToFile(const std::filesystem::path& filePath, const ImageParams& params = ImageParams());
			bool SaveToStream(Stream& stream, std::string_view format, const ImageParams& params = ImageParams());

			//TODO: SaveArray, SaveCubemap, SaveFace

			void SetLevelCount(UInt8 levelCount, bool allocateLevels = false);
			bool SetPixelColor(const Color& color, UInt32 x, UInt32 y = 0, UInt32 z = 0);

			using AbstractImage::Update;
			bool Update(const void* pixels, const Boxui32& box, UInt32 srcWidth = 0, UInt32 srcHeight = 0, UInt8 level = 0) override;

			Image& operator=(const Image& image);
			inline Image& operator=(Image&& image) noexcept;

			// Load
			static std::shared_ptr<Image> LoadFromFile(const std::filesystem::path& filePath, const ImageParams& params = ImageParams());
			static std::shared_ptr<Image> LoadFromMemory(const void* data, std::size_t size, const ImageParams& params = ImageParams());
			static std::shared_ptr<Image> LoadFromStream(Stream& stream, const ImageParams& params = ImageParams());

			// Load (array)
			static std::shared_ptr<Image> LoadFromFile(const std::filesystem::path& filePath, const ImageParams& imageParams, const Vector2ui32& atlasSize);
			static std::shared_ptr<Image> LoadFromImage(const Image& image, const Vector2ui32& atlasSize);
			static std::shared_ptr<Image> LoadFromMemory(const void* data, std::size_t size, const ImageParams& imageParams, const Vector2ui32& atlasSize);
			static std::shared_ptr<Image> LoadFromStream(Stream& stream, const ImageParams& imageParams, const Vector2ui32& atlasSize);

			// Load (cubemap)
			static std::shared_ptr<Image> LoadFromFile(const std::filesystem::path& filePath, const ImageParams& imageParams, const CubemapParams& cubemapParams);
			static std::shared_ptr<Image> LoadFromImage(const Image& image, const CubemapParams& params);
			static std::shared_ptr<Image> LoadFromMemory(const void* data, std::size_t size, const ImageParams& imageParams, const CubemapParams& cubemapParams);
			static std::shared_ptr<Image> LoadFromStream(Stream& stream, const ImageParams& imageParams, const CubemapParams& cubemapParams);

			struct SharedImage
			{
				using PixelContainer = FixedVector<std::unique_ptr<UInt8[]>, ImageUtils::MaxLevels>;

				SharedImage(unsigned short RefCount, ImageType Type, PixelFormat Format, PixelContainer&& Levels, UInt32 Width, UInt32 Height, UInt32 Depth) :
				refCount(RefCount),
				type(Type),
				format(Format),
				levels(std::move(Levels)),
				depth(Depth),
				height(Height),
				width(Width)
				{
				}

				std::atomic_ushort refCount;
				ImageType type;
				PixelFormat format;
				PixelContainer levels;
				UInt32 depth;
				UInt32 height;
				UInt32 width;
			};

			static SharedImage emptyImage;

		private:
			void EnsureOwnership();
			void ReleaseImage();

			SharedImage* m_sharedImage;
		};
}

#include <Nazara/Core/Image.inl>

#endif // NAZARA_CORE_IMAGE_HPP
