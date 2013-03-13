// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_IMAGE_HPP
#define NAZARA_IMAGE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Math/Cube.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/PixelFormat.hpp>

#if NAZARA_UTILITY_THREADSAFE && NAZARA_THREADSAFETY_IMAGE
#include <Nazara/Core/ThreadSafety.hpp>
#else
#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

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

using NzImageLoader = NzResourceLoader<NzImage, NzImageParams>;

class NAZARA_API NzImage : public NzResource
{
	friend NzImageLoader;

	public:
		struct SharedImage;

		NzImage();
		NzImage(nzImageType type, nzPixelFormat format, unsigned int width, unsigned int height, unsigned int depth = 1, nzUInt8 levelCount = 1);
		NzImage(const NzImage& image);
		NzImage(NzImage&& image) noexcept;
		NzImage(SharedImage* sharedImage);
		~NzImage();

		bool Convert(nzPixelFormat format);

		bool Copy(const NzImage& source, const NzCubeui& srcCube, const NzVector3ui& dstPos);

		bool Create(nzImageType type, nzPixelFormat format, unsigned int width, unsigned int height, unsigned int depth = 1, nzUInt8 levelCount = 1);
		void Destroy();

		bool Fill(const NzColor& color);
		bool Fill(const NzColor& color, const NzRectui& rect, unsigned int z = 0);
		bool Fill(const NzColor& color, const NzCubeui& cube);

		bool FlipHorizontally();
		bool FlipVertically();

		nzUInt8 GetBytesPerPixel() const;
		const nzUInt8* GetConstPixels(unsigned int x = 0, unsigned int y = 0, unsigned int z = 0, nzUInt8 level = 0) const;
		unsigned int GetDepth(nzUInt8 level = 0) const;
		nzPixelFormat GetFormat() const;
		unsigned int GetHeight(nzUInt8 level = 0) const;
		nzUInt8 GetLevelCount() const;
		nzUInt8 GetMaxLevel() const;
		NzColor GetPixelColor(unsigned int x, unsigned int y = 0, unsigned int z = 0) const;
		nzUInt8* GetPixels(unsigned int x = 0, unsigned int y = 0, unsigned int z = 0, nzUInt8 level = 0);
		unsigned int GetSize() const;
		unsigned int GetSize(nzUInt8 level) const;
		nzImageType GetType() const;
		unsigned int GetWidth(nzUInt8 level = 0) const;

		bool IsCompressed() const;
		bool IsCubemap() const;
		bool IsValid() const;

		bool LoadFromFile(const NzString& filePath, const NzImageParams& params = NzImageParams());
		bool LoadFromMemory(const void* data, std::size_t size, const NzImageParams& params = NzImageParams());
		bool LoadFromStream(NzInputStream& stream, const NzImageParams& params = NzImageParams());

		bool SetLevelCount(nzUInt8 levelCount);
		bool SetPixelColor(const NzColor& color, unsigned int x, unsigned int y = 0, unsigned int z = 0);

		bool Update(const nzUInt8* pixels, nzUInt8 level = 0);
		bool Update(const nzUInt8* pixels, const NzRectui& rect, unsigned int z = 0, nzUInt8 level = 0);
		bool Update(const nzUInt8* pixels, const NzCubeui& cube, nzUInt8 level = 0);

		NzImage& operator=(const NzImage& image);
		NzImage& operator=(NzImage&& image) noexcept;

		static nzUInt8 GetMaxLevel(unsigned int width, unsigned int height, unsigned int depth = 1);

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

			unsigned short refCount = 1;
			NazaraMutex(mutex)
		};

		static SharedImage emptyImage;

	private:
		void EnsureOwnership();
		void ReleaseImage();

		SharedImage* m_sharedImage;

		static NzImageLoader::LoaderList s_loaders;
};

#endif // NAZARA_IMAGE_HPP
