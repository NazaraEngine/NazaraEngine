// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_IMAGE_HPP
#define NAZARA_IMAGE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectListenerWrapper.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Utility/AbstractImage.hpp>
#include <Nazara/Utility/CubemapParams.hpp>
#include <atomic>

///TODO: Filtres

struct NAZARA_API NzImageParams
{
	// Le format dans lequel l'image doit être chargée (Undefined pour le format le plus proche de l'original)
	nzPixelFormat loadFormat = nzPixelFormat_Undefined;

	// Le nombre de niveaux de mipmaps maximum devant être créé
	nzUInt8 levelCount = 0;

	bool IsValid() const;
};

class NzImage;

using NzImageConstListener = NzObjectListenerWrapper<const NzImage>;
using NzImageConstRef = NzObjectRef<const NzImage>;
using NzImageLibrary = NzObjectLibrary<NzImage>;
using NzImageListener = NzObjectListenerWrapper<NzImage>;
using NzImageLoader = NzResourceLoader<NzImage, NzImageParams>;
using NzImageManager = NzResourceManager<NzImage, NzImageParams>;
using NzImageRef = NzObjectRef<NzImage>;

class NAZARA_API NzImage : public NzAbstractImage, public NzRefCounted, public NzResource
{
	friend NzImageLibrary;
	friend NzImageLoader;
	friend NzImageManager;
	friend class NzUtility;

	public:
		struct SharedImage;

		NzImage();
		NzImage(nzImageType type, nzPixelFormat format, unsigned int width, unsigned int height, unsigned int depth = 1, nzUInt8 levelCount = 1);
		NzImage(const NzImage& image);
		NzImage(SharedImage* sharedImage);
		~NzImage();

		bool Convert(nzPixelFormat format);

		void Copy(const NzImage& source, const NzBoxui& srcBox, const NzVector3ui& dstPos);

		bool Create(nzImageType type, nzPixelFormat format, unsigned int width, unsigned int height, unsigned int depth = 1, nzUInt8 levelCount = 1);
		void Destroy();

		bool Fill(const NzColor& color);
		bool Fill(const NzColor& color, const NzBoxui& box);
		bool Fill(const NzColor& color, const NzRectui& rect, unsigned int z = 0);

		bool FlipHorizontally();
		bool FlipVertically();

		const nzUInt8* GetConstPixels(unsigned int x = 0, unsigned int y = 0, unsigned int z = 0, nzUInt8 level = 0) const;
		unsigned int GetDepth(nzUInt8 level = 0) const;
		nzPixelFormat GetFormat() const;
		unsigned int GetHeight(nzUInt8 level = 0) const;
		nzUInt8 GetLevelCount() const;
		nzUInt8 GetMaxLevel() const;
		unsigned int GetMemoryUsage() const;
		unsigned int GetMemoryUsage(nzUInt8 level) const;
		NzColor GetPixelColor(unsigned int x, unsigned int y = 0, unsigned int z = 0) const;
		nzUInt8* GetPixels(unsigned int x = 0, unsigned int y = 0, unsigned int z = 0, nzUInt8 level = 0);
		NzVector3ui GetSize(nzUInt8 level = 0) const;
		nzImageType GetType() const;
		unsigned int GetWidth(nzUInt8 level = 0) const;

		bool IsValid() const;

		// Load
		bool LoadFromFile(const NzString& filePath, const NzImageParams& params = NzImageParams());
		bool LoadFromMemory(const void* data, std::size_t size, const NzImageParams& params = NzImageParams());
		bool LoadFromStream(NzInputStream& stream, const NzImageParams& params = NzImageParams());

		// LoadArray
		bool LoadArrayFromFile(const NzString& filePath, const NzImageParams& imageParams = NzImageParams(), const NzVector2ui& atlasSize = NzVector2ui(2, 2));
		bool LoadArrayFromImage(const NzImage& image, const NzVector2ui& atlasSize = NzVector2ui(2, 2));
		bool LoadArrayFromMemory(const void* data, std::size_t size, const NzImageParams& imageParams = NzImageParams(), const NzVector2ui& atlasSize = NzVector2ui(2, 2));
		bool LoadArrayFromStream(NzInputStream& stream, const NzImageParams& imageParams = NzImageParams(), const NzVector2ui& atlasSize = NzVector2ui(2, 2));

		// LoadCubemap
		bool LoadCubemapFromFile(const NzString& filePath, const NzImageParams& imageParams = NzImageParams(), const NzCubemapParams& cubemapParams = NzCubemapParams());
		bool LoadCubemapFromImage(const NzImage& image, const NzCubemapParams& params = NzCubemapParams());
		bool LoadCubemapFromMemory(const void* data, std::size_t size, const NzImageParams& imageParams = NzImageParams(), const NzCubemapParams& cubemapParams = NzCubemapParams());
		bool LoadCubemapFromStream(NzInputStream& stream, const NzImageParams& imageParams = NzImageParams(), const NzCubemapParams& cubemapParams = NzCubemapParams());

		void SetLevelCount(nzUInt8 levelCount);
		bool SetPixelColor(const NzColor& color, unsigned int x, unsigned int y = 0, unsigned int z = 0);

		bool Update(const nzUInt8* pixels, unsigned int srcWidth = 0, unsigned int srcHeight = 0, nzUInt8 level = 0);
		bool Update(const nzUInt8* pixels, const NzBoxui& box, unsigned int srcWidth = 0, unsigned int srcHeight = 0, nzUInt8 level = 0);
		bool Update(const nzUInt8* pixels, const NzRectui& rect, unsigned int z = 0, unsigned int srcWidth = 0, unsigned int srcHeight = 0, nzUInt8 level = 0);

		NzImage& operator=(const NzImage& image);

		static void Copy(nzUInt8* destination, const nzUInt8* source, nzUInt8 bpp, unsigned int width, unsigned int height, unsigned int depth = 1, unsigned int dstWidth = 0, unsigned int dstHeight = 0, unsigned int srcWidth = 0, unsigned int srcHeight = 0);
		static nzUInt8 GetMaxLevel(unsigned int width, unsigned int height, unsigned int depth = 1);
		static nzUInt8 GetMaxLevel(nzImageType type, unsigned int width, unsigned int height, unsigned int depth = 1);
		template<typename... Args> static NzImageRef New(Args&&... args);

		struct SharedImage
		{
			SharedImage(unsigned short RefCount, nzImageType Type, nzPixelFormat Format, nzUInt8 LevelCount = 1, nzUInt8** Pixels = nullptr, unsigned int Width = 1, unsigned int Height = 1, unsigned int Depth = 1) :
			type(Type),
			format(Format),
			levelCount(LevelCount),
			pixels(Pixels),
			depth(Depth),
			height(Height),
			width(Width),
			refCount(RefCount)
			{
			}

			nzImageType type;
			nzPixelFormat format;
			nzUInt8 levelCount;
			nzUInt8** pixels;
			unsigned int depth;
			unsigned int height;
			unsigned int width;

			std::atomic_ushort refCount;
		};

		static SharedImage emptyImage;

	private:
		void EnsureOwnership();
		void ReleaseImage();

		static bool Initialize();
		static void Uninitialize();

		SharedImage* m_sharedImage;

		static NzImageLibrary::LibraryMap s_library;
		static NzImageLoader::LoaderList s_loaders;
		static NzImageManager::ManagerMap s_managerMap;
		static NzImageManager::ManagerParams s_managerParameters;
};

#include <Nazara/Utility/Image.inl>

#endif // NAZARA_IMAGE_HPP
