// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_IMAGE_HPP
#define NAZARA_IMAGE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Math/Cube.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Utility/ResourceLoader.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Utility/Resource.hpp>
//#include <Nazara/Utility/ThreadSafety.hpp>

enum nzCubemapFace
{
	nzCubemapFace_PositiveX,
	nzCubemapFace_NegativeX,
	nzCubemapFace_PositiveY,
	nzCubemapFace_NegativeY,
	nzCubemapFace_PositiveZ,
	nzCubemapFace_NegativeZ
};

enum nzImageType
{
	nzImageType_1D,
	nzImageType_2D,
	nzImageType_3D,
	nzImageType_Cubemap,

	nzImageType_Count
};

struct NzImageParams
{
	// GCC 4.7 je te veux
	NzImageParams() :
	loadFormat(nzPixelFormat_Undefined),
	levelCount(0)
	{
	}

	nzPixelFormat loadFormat;
	nzUInt8 levelCount;

	bool IsValid() const
	{
		return loadFormat == nzPixelFormat_Undefined || NzPixelFormat::IsValid(loadFormat);
	}
};

///TODO: Filtres

class NAZARA_API NzImage : public NzResource, public NzResourceLoader<NzImage, NzImageParams>
{
	public:
		struct SharedImage;

		NzImage();
		NzImage(const NzImage& image);
		NzImage(NzImage&& image);
		NzImage(SharedImage* sharedImage);
		~NzImage();

		bool Convert(nzPixelFormat format);

		bool Copy(const NzImage& source, const NzRectui& srcRect, const NzVector2ui& dstPos);
		bool CopyToFace(nzCubemapFace face, const NzImage& source, const NzRectui& srcRect, const NzVector2ui& dstPos);

		bool Create(nzImageType type, nzPixelFormat format, unsigned int width, unsigned int height, unsigned int depth = 1, nzUInt8 levelCount = 1);
		void Destroy();

		nzUInt8 GetBPP() const;
		const nzUInt8* GetConstPixels(nzUInt8 level = 0) const;
		unsigned int GetDepth(nzUInt8 level = 0) const;
		nzPixelFormat GetFormat() const;
		unsigned int GetHeight(nzUInt8 level = 0) const;
		nzUInt8 GetLevelCount() const;
		nzUInt8 GetMaxLevel() const;
		NzColor GetPixel(unsigned int x, unsigned int y = 0, unsigned int z = 0) const;
		NzColor GetPixelFace(nzCubemapFace face, unsigned int x, unsigned int y) const;
		nzUInt8* GetPixels(nzUInt8 level = 0);
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
		bool SetPixel(const NzColor& color, unsigned int x, unsigned int y = 0, unsigned int z = 0);
		bool SetPixelFace(nzCubemapFace face, const NzColor& color, unsigned int x, unsigned int y);

		bool Update(const nzUInt8* pixels, nzUInt8 level = 0);
		bool Update(const nzUInt8* pixels, const NzRectui& rect, unsigned int z = 0, nzUInt8 level = 0);
		bool Update(const nzUInt8* pixels, const NzCubeui& cube, nzUInt8 level = 0);
		bool UpdateFace(nzCubemapFace face, const nzUInt8* pixels, nzUInt8 level = 0);
		bool UpdateFace(nzCubemapFace face, const nzUInt8* pixels, const NzRectui& rect, nzUInt8 level = 0);

		NzImage& operator=(const NzImage& image);
		NzImage& operator=(NzImage&& image);

		static nzUInt8 GetMaxLevel(unsigned int width, unsigned int height, unsigned int depth = 1);
		static void RegisterFileLoader(const NzString& extensions, LoadFileFunction loadFile);
		static void RegisterMemoryLoader(IsMemoryLoadingSupportedFunction isLoadingSupported, LoadMemoryFunction loadMemory);
		static void RegisterStreamLoader(IsStreamLoadingSupportedFunction isLoadingSupported, LoadStreamFunction loadStream);
		static void UnregisterFileLoader(const NzString& extensions, LoadFileFunction loadFile);
		static void UnregisterMemoryLoader(IsMemoryLoadingSupportedFunction isLoadingSupported, LoadMemoryFunction loadMemory);
		static void UnregisterStreamLoader(IsStreamLoadingSupportedFunction isLoadingSupported, LoadStreamFunction loadStream);

		struct SharedImage
		{
			SharedImage() : // Vivement GCC 4.7 sur Windows
			refCount(1)
			{
			}

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

			unsigned short refCount;
			NazaraMutex(mutex)
		};

		static SharedImage emptyImage;

	private:
		void EnsureOwnership();
		void ReleaseImage();

		SharedImage* m_sharedImage;
};

#endif // NAZARA_IMAGE_HPP
