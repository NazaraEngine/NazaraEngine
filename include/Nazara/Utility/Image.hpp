// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_IMAGE_HPP
#define NAZARA_IMAGE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
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
		PixelFormatType loadFormat = PixelFormatType_Undefined;

		// Le nombre de niveaux de mipmaps maximum devant être créé
		UInt8 levelCount = 0;

		bool IsValid() const;
	};

	class Image;

	using ImageConstRef = ObjectRef<const Image>;
	using ImageLibrary = ObjectLibrary<Image>;
	using ImageLoader = ResourceLoader<Image, ImageParams>;
	using ImageManager = ResourceManager<Image, ImageParams>;
	using ImageRef = ObjectRef<Image>;
	using ImageSaver = ResourceSaver<Image, ImageParams>;

	class NAZARA_UTILITY_API Image : public AbstractImage, public Resource
	{
		friend ImageLibrary;
		friend ImageLoader;
		friend ImageManager;
		friend ImageSaver;
		friend class Utility;

		public:
			struct SharedImage;

			Image();
			Image(ImageType type, PixelFormatType format, unsigned int width, unsigned int height, unsigned int depth = 1, UInt8 levelCount = 1);
			Image(const Image& image);
			Image(SharedImage* sharedImage);
			~Image();

			bool Convert(PixelFormatType format);

			void Copy(const Image& source, const Boxui& srcBox, const Vector3ui& dstPos);

			bool Create(ImageType type, PixelFormatType format, unsigned int width, unsigned int height, unsigned int depth = 1, UInt8 levelCount = 1);
			void Destroy();

			bool Fill(const Color& color);
			bool Fill(const Color& color, const Boxui& box);
			bool Fill(const Color& color, const Rectui& rect, unsigned int z = 0);

			bool FlipHorizontally();
			bool FlipVertically();

			const UInt8* GetConstPixels(unsigned int x = 0, unsigned int y = 0, unsigned int z = 0, UInt8 level = 0) const;
			unsigned int GetDepth(UInt8 level = 0) const override;
			PixelFormatType GetFormat() const override;
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

			// Load
			bool LoadFromFile(const String& filePath, const ImageParams& params = ImageParams());
			bool LoadFromMemory(const void* data, std::size_t size, const ImageParams& params = ImageParams());
			bool LoadFromStream(Stream& stream, const ImageParams& params = ImageParams());

			// LoadArray
			bool LoadArrayFromFile(const String& filePath, const ImageParams& imageParams = ImageParams(), const Vector2ui& atlasSize = Vector2ui(2, 2));
			bool LoadArrayFromImage(const Image& image, const Vector2ui& atlasSize = Vector2ui(2, 2));
			bool LoadArrayFromMemory(const void* data, std::size_t size, const ImageParams& imageParams = ImageParams(), const Vector2ui& atlasSize = Vector2ui(2, 2));
			bool LoadArrayFromStream(Stream& stream, const ImageParams& imageParams = ImageParams(), const Vector2ui& atlasSize = Vector2ui(2, 2));

			// LoadCubemap
			bool LoadCubemapFromFile(const String& filePath, const ImageParams& imageParams = ImageParams(), const CubemapParams& cubemapParams = CubemapParams());
			bool LoadCubemapFromImage(const Image& image, const CubemapParams& params = CubemapParams());
			bool LoadCubemapFromMemory(const void* data, std::size_t size, const ImageParams& imageParams = ImageParams(), const CubemapParams& cubemapParams = CubemapParams());
			bool LoadCubemapFromStream(Stream& stream, const ImageParams& imageParams = ImageParams(), const CubemapParams& cubemapParams = CubemapParams());

			// LoadFace
			bool LoadFaceFromFile(CubemapFace face, const String& filePath, const ImageParams& params = ImageParams());
			bool LoadFaceFromMemory(CubemapFace face, const void* data, std::size_t size, const ImageParams& params = ImageParams());
			bool LoadFaceFromStream(CubemapFace face, Stream& stream, const ImageParams& params = ImageParams());

			// Save
			bool SaveToFile(const String& filePath, const ImageParams& params = ImageParams());
			bool SaveToStream(Stream& stream, const String& format, const ImageParams& params = ImageParams());

			//TODO: SaveArray, SaveCubemap, SaveFace

			void SetLevelCount(UInt8 levelCount);
			bool SetPixelColor(const Color& color, unsigned int x, unsigned int y = 0, unsigned int z = 0);

			bool Update(const UInt8* pixels, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0) override;
			bool Update(const UInt8* pixels, const Boxui& box, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0) override;
			bool Update(const UInt8* pixels, const Rectui& rect, unsigned int z = 0, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0) override;

			Image& operator=(const Image& image);

			static void Copy(UInt8* destination, const UInt8* source, PixelFormatType format, unsigned int width, unsigned int height, unsigned int depth = 1, unsigned int dstWidth = 0, unsigned int dstHeight = 0, unsigned int srcWidth = 0, unsigned int srcHeight = 0);
			static UInt8 GetMaxLevel(unsigned int width, unsigned int height, unsigned int depth = 1);
			static UInt8 GetMaxLevel(ImageType type, unsigned int width, unsigned int height, unsigned int depth = 1);
			template<typename... Args> static ImageRef New(Args&&... args);

			struct SharedImage
			{
				using PixelContainer = std::vector<std::unique_ptr<UInt8[]>>;

				SharedImage(unsigned short RefCount, ImageType Type, PixelFormatType Format, PixelContainer&& Levels, unsigned int Width, unsigned int Height, unsigned int Depth) :
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
				PixelFormatType format;
				PixelContainer levels;
				unsigned int depth;
				unsigned int height;
				unsigned int width;

				std::atomic_ushort refCount;
			};

			static SharedImage emptyImage;

			// Signals:
			NazaraSignal(OnImageDestroy, const Image* /*image*/);
			NazaraSignal(OnImageRelease, const Image* /*image*/);

		private:
			void EnsureOwnership();
			void ReleaseImage();

			static bool Initialize();
			static void Uninitialize();

			SharedImage* m_sharedImage;

			static ImageLibrary::LibraryMap s_library;
			static ImageLoader::LoaderList s_loaders;
			static ImageManager::ManagerMap s_managerMap;
			static ImageManager::ManagerParams s_managerParameters;
			static ImageSaver::SaverList s_savers;
		};
}

#include <Nazara/Utility/Image.inl>

#endif // NAZARA_IMAGE_HPP
