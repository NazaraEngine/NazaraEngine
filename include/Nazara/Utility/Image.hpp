// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_IMAGE_HPP
#define NAZARA_IMAGE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/InputStream.hpp>
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
	nzImageType_Cubemap
};

struct NzImageParams
{
	bool IsValid() const
	{
		return true;
	}
};

///TODO: Filtres
///TODO: Mipmaps

class NAZARA_API NzImage : public NzResource, public NzResourceLoader<NzImage, NzImageParams>
{
	public:
		struct SharedImage;

		NzImage();
		NzImage(const NzImage& image);
		NzImage(NzImage&& image);
		NzImage(SharedImage* sharedImage);
		~NzImage();

		bool Copy(const NzImage& source, const NzRectui& srcRect, const NzVector2ui& dstPos);
		bool CopyToFace(nzCubemapFace face, const NzImage& source, const NzRectui& srcRect, const NzVector2ui& dstPos);

		bool Create(nzImageType type, nzPixelFormat format, unsigned int width, unsigned int height = 1, unsigned int depth = 1);
		void Destroy();

		nzUInt8 GetBPP() const;
		const nzUInt8* GetConstPixels() const;
		unsigned int GetDepth() const;
		nzPixelFormat GetFormat() const;
		unsigned int GetHeight() const;
		nzUInt8* GetPixels();
		unsigned int GetSize() const;
		nzImageType GetType() const;
		unsigned int GetWidth() const;

		bool IsCompressed() const;
		bool IsCubemap() const;
		bool IsValid() const;

		bool LoadFromFile(const NzString& filePath, const NzImageParams& params = NzImageParams());
		bool LoadFromMemory(const void* data, std::size_t size, const NzImageParams& params = NzImageParams());
		bool LoadFromStream(NzInputStream& stream, const NzImageParams& params = NzImageParams());

		bool Update(const nzUInt8* pixels);
		bool Update(const nzUInt8* pixels, const NzRectui& rect);
		bool UpdateFace(nzCubemapFace face, const nzUInt8* pixels);
		bool UpdateFace(nzCubemapFace face, const nzUInt8* pixels, const NzRectui& rect);

		NzImage& operator=(const NzImage& image);
		NzImage& operator=(NzImage&& image);

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

			SharedImage(unsigned short RefCount, nzImageType Type, nzPixelFormat Format, nzUInt8* Pixels, unsigned int Width, unsigned int Height = 1, unsigned int Depth = 1) :
			type(Type),
			format(Format),
			pixels(Pixels),
			depth(Depth),
			height(Height),
			width(Width),
			refCount(RefCount)
			{
			}

			nzImageType type;
			nzPixelFormat format;
			nzUInt8* pixels;
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
