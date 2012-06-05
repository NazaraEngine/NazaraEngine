// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TEXTURE_HPP
#define NAZARA_TEXTURE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/PixelFormat.hpp>

enum nzTextureFilter
{
	nzTextureFilter_Bilinear,
	nzTextureFilter_Nearest,
	nzTextureFilter_Trilinear,
	nzTextureFilter_Unknown
};

enum nzTextureWrap
{
	nzTextureWrap_Clamp,
	nzTextureWrap_Repeat,
	nzTextureWrap_Unknown
};

class NzShader;
struct NzTextureImpl;

class NAZARA_API NzTexture : public NzResource, NzNonCopyable
{
	friend class NzShader;

	public:
		NzTexture();
		explicit NzTexture(const NzImage& image);
		~NzTexture();

		bool Bind();

		bool Create(nzImageType type, nzPixelFormat format, unsigned int width, unsigned int height, unsigned int depth = 1, nzUInt8 levelCount = 1, bool lock = false);
		void Destroy();

		bool Download(NzImage* image) const;

		bool EnableMipmapping(bool enable);

		unsigned int GetAnisotropyLevel() const;
		nzUInt8 GetBPP() const;
		unsigned int GetDepth() const;
		nzTextureFilter GetFilterMode() const;
		nzPixelFormat GetFormat() const;
		unsigned int GetHeight() const;
		nzImageType GetType() const;
		unsigned int GetWidth() const;
		nzTextureWrap GetWrapMode() const;

		bool IsCompressed() const;
		bool IsCubemap() const;
		bool IsValid() const;

		bool LoadFromFile(const NzString& filePath, const NzImageParams& params = NzImageParams());
		bool LoadFromImage(const NzImage& image);
		bool LoadFromMemory(const void* data, std::size_t size, const NzImageParams& params = NzImageParams());
		bool LoadFromStream(NzInputStream& stream, const NzImageParams& params = NzImageParams());

		bool Lock();

		bool SetAnisotropyLevel(unsigned int anistropyLevel);
		bool SetFilterMode(nzTextureFilter filter);
		bool SetMipmapRange(nzUInt8 minLevel, nzUInt8 maxLevel);
		bool SetWrapMode(nzTextureWrap wrap);

		bool Update(const NzImage& image, nzUInt8 level = 0);
		bool Update(const NzImage& image, const NzRectui& rect, unsigned int z = 0, nzUInt8 level = 0);
		//bool Update(const NzImage& image, const NzCubeui& cube, nzUInt8 level = 0);
		bool Update(const nzUInt8* pixels, nzUInt8 level = 0);
		bool Update(const nzUInt8* pixels, const NzRectui& rect, unsigned int z = 0, nzUInt8 level = 0);
		//bool Update(const nzUInt8* pixels, const NzCubeui& cube, nzUInt8 level = 0);
		bool UpdateFace(nzCubemapFace face, const NzImage& image, nzUInt8 level = 0);
		bool UpdateFace(nzCubemapFace face, const NzImage& image, const NzRectui& rect, nzUInt8 level = 0);
		bool UpdateFace(nzCubemapFace face, const nzUInt8* pixels, nzUInt8 level = 0);
		bool UpdateFace(nzCubemapFace face, const nzUInt8* pixels, const NzRectui& rect, nzUInt8 level = 0);

		void Unlock();

		static unsigned int GetValidSize(unsigned int size);
		static bool IsFormatSupported(nzPixelFormat format);
		static bool IsTypeSupported(nzImageType type);

	private:
		NzTextureImpl* m_impl;
};

#endif // NAZARA_TEXTURE_HPP
