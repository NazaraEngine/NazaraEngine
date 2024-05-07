// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_TEXTURE_HPP
#define NAZARA_RENDERER_TEXTURE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Image.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/Export.hpp>

namespace Nz
{
	class RenderDevice;

	struct TextureInfo
	{
		PixelFormat pixelFormat;
		ImageType type;
		TextureUsageFlags usageFlags = TextureUsage::ShaderSampling | TextureUsage::TransferDestination | TextureUsage::TransferSource;
		UInt8 levelCount = 0xFF;
		UInt32 layerCount = 1;
		UInt32 depth = 1;
		UInt32 height;
		UInt32 width;
	};

	struct TextureViewInfo
	{
		ImageType viewType;
		PixelFormat reinterpretFormat = PixelFormat::Undefined;
		UInt8 baseMipLevel = 0;
		UInt8 levelCount = 1;
		UInt32 baseArrayLayer = 0;
		UInt32 layerCount = 1;
	};

	struct NAZARA_RENDERER_API TextureParams : ImageParams
	{
		std::shared_ptr<RenderDevice> renderDevice;
		TextureUsageFlags usageFlags = TextureUsage::ShaderSampling | TextureUsage::TransferDestination | TextureUsage::TransferSource;
		bool buildMipmaps = true;

		bool IsValid() const;
		void Merge(const TextureParams& params);
	};

	class Texture;

	using TextureLibrary = ObjectLibrary<Texture>;
	using TextureLoader = ResourceLoader<Texture, TextureParams>;
	using TextureManager = ResourceManager<Texture, TextureParams>;

	class NAZARA_RENDERER_API Texture : public AbstractImage, public Resource, public std::enable_shared_from_this<Texture> //< FIXME
	{
		public:
			using Params = TextureParams;

			Texture() = default;
			Texture(const Texture&) = delete;
			Texture(Texture&&) = delete;
			virtual ~Texture();

			virtual bool Copy(const Texture& source, const Boxui32& srcBox, const Vector3ui32& dstPos = Vector3ui32::Zero()) = 0;
			virtual std::shared_ptr<Texture> CreateView(const TextureViewInfo& viewInfo) = 0;

			virtual RenderDevice* GetDevice() = 0;
			virtual const RenderDevice* GetDevice() const = 0;
			virtual Texture* GetParentTexture() const = 0;
			virtual const TextureInfo& GetTextureInfo() const = 0;

			virtual void UpdateDebugName(std::string_view name) = 0;

			Texture& operator=(const Texture&) = delete;
			Texture& operator=(Texture&&) = delete;

			static inline TextureInfo ApplyView(TextureInfo textureInfo, const TextureViewInfo& viewInfo);
			static inline TextureInfo BuildTextureInfo(const Image& image);

			static std::shared_ptr<Texture> CreateFromImage(const Image& image, const TextureParams& params);

			// Load
			static std::shared_ptr<Texture> LoadFromFile(const std::filesystem::path& filePath, const TextureParams& params);
			static std::shared_ptr<Texture> LoadFromMemory(const void* data, std::size_t size, const TextureParams& params);
			static std::shared_ptr<Texture> LoadFromStream(Stream& stream, const TextureParams& params);

			// LoadArray
			static std::shared_ptr<Texture> LoadFromFile(const std::filesystem::path& filePath, const TextureParams& textureParams, const Vector2ui32& atlasSize);
			static std::shared_ptr<Texture> LoadFromMemory(const void* data, std::size_t size, const TextureParams& textureParams, const Vector2ui32& atlasSize);
			static std::shared_ptr<Texture> LoadFromStream(Stream& stream, const TextureParams& textureParams, const Vector2ui32& atlasSize);

			// LoadCubemap
			static std::shared_ptr<Texture> LoadFromFile(const std::filesystem::path& filePath, const TextureParams& textureParams, const CubemapParams& cubemapParams);
			static std::shared_ptr<Texture> LoadFromMemory(const void* data, std::size_t size, const TextureParams& textureParams, const CubemapParams& cubemapParams);
			static std::shared_ptr<Texture> LoadFromStream(Stream& stream, const TextureParams& textureParams, const CubemapParams& cubemapParams);
	};
}

#include <Nazara/Renderer/Texture.inl>

#endif // NAZARA_RENDERER_TEXTURE_HPP
