// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TEXTURE_HPP
#define NAZARA_TEXTURE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceRef.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Utility/CubemapParams.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/PixelFormat.hpp>

class NzTexture;

using NzTextureConstRef = NzResourceRef<const NzTexture>;
using NzTextureRef = NzResourceRef<NzTexture>;

struct NzTextureImpl;

class NAZARA_API NzTexture : public NzResource, NzNonCopyable
{
	friend class NzRenderer;
	friend class NzRenderTexture;

	public:
		NzTexture() = default;
		explicit NzTexture(const NzImage& image);
		~NzTexture();

		bool Create(nzImageType type, nzPixelFormat format, unsigned int width, unsigned int height, unsigned int depth = 1, nzUInt8 levelCount = 1);
		void Destroy();

		bool Download(NzImage* image) const;

		bool EnableMipmapping(bool enable);

		void EnsureMipmapsUpdate() const;

		nzUInt8 GetBytesPerPixel() const;
		unsigned int GetDepth() const;
		nzPixelFormat GetFormat() const;
		unsigned int GetHeight() const;
		NzVector2ui GetSize() const;
		nzImageType GetType() const;
		unsigned int GetWidth() const;

		bool HasMipmaps() const;

		bool IsCompressed() const;
		bool IsCubemap() const;
		bool IsValid() const;

		// Load
		bool LoadFromFile(const NzString& filePath, const NzImageParams& params = NzImageParams(), bool generateMipmaps = true);
		bool LoadFromImage(const NzImage& image, bool generateMipmaps = true);
		bool LoadFromMemory(const void* data, std::size_t size, const NzImageParams& params = NzImageParams(), bool generateMipmaps = true);
		bool LoadFromStream(NzInputStream& stream, const NzImageParams& params = NzImageParams(), bool generateMipmaps = true);

		// LoadArray
		bool LoadArrayFromFile(const NzString& filePath, const NzImageParams& imageParams = NzImageParams(), bool generateMipmaps = true, const NzVector2ui& atlasSize = NzVector2ui(2, 2));
		bool LoadArrayFromImage(const NzImage& image, bool generateMipmaps = true, const NzVector2ui& atlasSize = NzVector2ui(2, 2));
		bool LoadArrayFromMemory(const void* data, std::size_t size, const NzImageParams& imageParams = NzImageParams(), bool generateMipmaps = true, const NzVector2ui& atlasSize = NzVector2ui(2, 2));
		bool LoadArrayFromStream(NzInputStream& stream, const NzImageParams& imageParams = NzImageParams(), bool generateMipmaps = true, const NzVector2ui& atlasSize = NzVector2ui(2, 2));

		// LoadCubemap
		bool LoadCubemapFromFile(const NzString& filePath, const NzImageParams& imageParams = NzImageParams(), bool generateMipmaps = true, const NzCubemapParams& cubemapParams = NzCubemapParams());
		bool LoadCubemapFromImage(const NzImage& image, bool generateMipmaps = true, const NzCubemapParams& params = NzCubemapParams());
		bool LoadCubemapFromMemory(const void* data, std::size_t size, const NzImageParams& imageParams = NzImageParams(), bool generateMipmaps = true, const NzCubemapParams& cubemapParams = NzCubemapParams());
		bool LoadCubemapFromStream(NzInputStream& stream, const NzImageParams& imageParams = NzImageParams(), bool generateMipmaps = true, const NzCubemapParams& cubemapParams = NzCubemapParams());

		// LoadFace
		bool LoadFaceFromFile(nzCubemapFace face, const NzString& filePath, const NzImageParams& params = NzImageParams());
		bool LoadFaceFromMemory(nzCubemapFace face, const void* data, std::size_t size, const NzImageParams& params = NzImageParams());
		bool LoadFaceFromStream(nzCubemapFace face, NzInputStream& stream, const NzImageParams& params = NzImageParams());

		bool SetMipmapRange(nzUInt8 minLevel, nzUInt8 maxLevel);

		bool Update(const NzImage& image, nzUInt8 level = 0);
		bool Update(const NzImage& image, const NzBoxui& box, nzUInt8 level = 0);
		bool Update(const NzImage& image, const NzRectui& rect, unsigned int z = 0, nzUInt8 level = 0);
		bool Update(const nzUInt8* pixels, unsigned int srcWidth = 0, unsigned int srcHeight = 0, nzUInt8 level = 0);
		bool Update(const nzUInt8* pixels, const NzBoxui& box, unsigned int srcWidth = 0, unsigned int srcHeight = 0, nzUInt8 level = 0);
		bool Update(const nzUInt8* pixels, const NzRectui& rect, unsigned int z = 0, unsigned int srcWidth = 0, unsigned int srcHeight = 0, nzUInt8 level = 0);
		bool UpdateFace(nzCubemapFace face, const NzImage& image, nzUInt8 level = 0);
		bool UpdateFace(nzCubemapFace face, const NzImage& image, const NzRectui& rect, nzUInt8 level = 0);
		bool UpdateFace(nzCubemapFace face, const nzUInt8* pixels, unsigned int srcWidth = 0, unsigned int srcHeight = 0, nzUInt8 level = 0);
		bool UpdateFace(nzCubemapFace face, const nzUInt8* pixels, const NzRectui& rect, unsigned int srcWidth = 0, unsigned int srcHeight = 0, nzUInt8 level = 0);

		// Fonctions OpenGL
		unsigned int GetOpenGLID() const;

		static unsigned int GetValidSize(unsigned int size);
		static bool IsFormatSupported(nzPixelFormat format);
		static bool IsMipmappingSupported();
		static bool IsTypeSupported(nzImageType type);

	private:
		void InvalidateMipmaps();

		NzTextureImpl* m_impl = nullptr;
};

#endif // NAZARA_TEXTURE_HPP
