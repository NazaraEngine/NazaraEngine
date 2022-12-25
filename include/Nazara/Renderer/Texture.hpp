// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_TEXTURE_HPP
#define NAZARA_RENDERER_TEXTURE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Utility/Image.hpp>

namespace Nz
{
	class RenderDevice;

	struct TextureInfo
	{
		PixelFormat pixelFormat;
		ImageType type;
		TextureUsageFlags usageFlags = TextureUsage::ShaderSampling | TextureUsage::TransferDestination;
		UInt8 levelCount = 1;
		unsigned int layerCount = 1;
		unsigned int depth = 1;
		unsigned int height;
		unsigned int width;
	};

	struct TextureViewInfo
	{
		ImageType viewType;
		PixelFormat reinterpretFormat;
		UInt8 baseMipLevel = 0;
		UInt8 levelCount = 1;
		unsigned int baseArrayLayer = 0;
		unsigned int layerCount = 1;
	};

	struct NAZARA_RENDERER_API TextureParams : ImageParams
	{
		std::shared_ptr<RenderDevice> renderDevice;
		TextureUsageFlags usageFlags = TextureUsage::ShaderSampling | TextureUsage::TransferDestination;

		bool IsValid() const;
	};

	class Texture;

	using TextureLibrary = ObjectLibrary<Texture>;
	using TextureLoader = ResourceLoader<Texture, TextureParams>;
	using TextureManager = ResourceManager<Texture, TextureParams>;

	class NAZARA_RENDERER_API Texture : public AbstractImage, public Resource, public std::enable_shared_from_this<Texture> //< FIXME
	{
		public:
			Texture() = default;
			Texture(const Texture&) = delete;
			Texture(Texture&&) = delete;
			virtual ~Texture();

			virtual bool Copy(const Texture& source, const Boxui& srcBox, const Vector3ui& dstPos = Vector3ui::Zero()) = 0;
			virtual std::shared_ptr<Texture> CreateView(const TextureViewInfo& viewInfo) = 0;

			virtual Texture* GetParentTexture() const = 0;
			virtual const TextureInfo& GetTextureInfo() const = 0;

			virtual void UpdateDebugName(std::string_view name) = 0;

			Texture& operator=(const Texture&) = delete;
			Texture& operator=(Texture&&) = delete;

			static inline TextureInfo ApplyView(TextureInfo textureInfo, const TextureViewInfo& viewInfo);
			static inline unsigned int GetLevelSize(unsigned int size, unsigned int level);

			static std::shared_ptr<Texture> CreateFromImage(const Image& image, const TextureParams& params);

			// Load
			static std::shared_ptr<Texture> LoadFromFile(const std::filesystem::path& filePath, const TextureParams& params);
			static std::shared_ptr<Texture> LoadFromMemory(const void* data, std::size_t size, const TextureParams& params);
			static std::shared_ptr<Texture> LoadFromStream(Stream& stream, const TextureParams& params);

			// LoadArray
			static std::shared_ptr<Texture> LoadArrayFromFile(const std::filesystem::path& filePath, const TextureParams& textureParams, const Vector2ui& atlasSize = Vector2ui(2, 2));
			static std::shared_ptr<Texture> LoadArrayFromMemory(const void* data, std::size_t size, const TextureParams& textureParams, const Vector2ui& atlasSize = Vector2ui(2, 2));
			static std::shared_ptr<Texture> LoadArrayFromStream(Stream& stream, const TextureParams& textureParams, const Vector2ui& atlasSize = Vector2ui(2, 2));

			// LoadCubemap
			static std::shared_ptr<Texture> LoadCubemapFromFile(const std::filesystem::path& filePath, const TextureParams& textureParams, const CubemapParams& cubemapParams = CubemapParams());
			static std::shared_ptr<Texture> LoadCubemapFromMemory(const void* data, std::size_t size, const TextureParams& textureParams, const CubemapParams& cubemapParams = CubemapParams());
			static std::shared_ptr<Texture> LoadCubemapFromStream(Stream& stream, const TextureParams& textureParams, const CubemapParams& cubemapParams = CubemapParams());
	};
}

#include <Nazara/Renderer/Texture.inl>

#endif // NAZARA_RENDERER_TEXTURE_HPP
