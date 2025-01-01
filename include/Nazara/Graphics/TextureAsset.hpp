// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_TEXTUREASSET_HPP
#define NAZARA_GRAPHICS_TEXTUREASSET_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/CubemapParams.hpp>
#include <Nazara/Core/Image.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <variant>

namespace Nz
{
	struct NAZARA_GRAPHICS_API TextureAssetParams : ResourceParameters
	{
		bool IsValid() const;
		void Merge(const TextureAssetParams& params);

		TextureUsageFlags usageFlags = TextureUsage::ShaderSampling | TextureUsage::TransferDestination | TextureUsage::TransferSource;
		bool sRGB = false;
	};

	class TextureAsset;

	using TextureAssetLibrary = ObjectLibrary<TextureAsset>;
	using TextureAssetLoader = ResourceLoader<TextureAsset, TextureAssetParams>;
	using TextureAssetManager = ResourceManager<TextureAsset, TextureAssetParams>;

	class NAZARA_GRAPHICS_API TextureAsset : public Resource
	{
		public:
			using ImageBuilder = std::function<Image(RenderDevice& renderDevice, const TextureAssetParams& params)>;
			using TextureBuilder = std::function<std::shared_ptr<Texture>(RenderDevice& renderDevice, const TextureAssetParams& params)>;

			using Params = TextureAssetParams;

			TextureAsset() = default;
			TextureAsset(const TextureAsset&) = delete;
			TextureAsset(TextureAsset&&) = default;
			inline ~TextureAsset();

			bool Create(std::shared_ptr<Texture> texture);
			bool Create(std::shared_ptr<TextureAsset> textureAsset, const TextureViewInfo& viewInfo, const TextureAssetParams& params = TextureAssetParams{});
			bool Create(std::unique_ptr<Stream> imageStream, const TextureAssetParams& params = TextureAssetParams{});
			bool Create(std::unique_ptr<Stream> imageStream,  const TextureAssetParams& params, const Vector2ui32& atlasSize);
			bool Create(std::unique_ptr<Stream> imageStream,  const TextureAssetParams& params, const CubemapParams& cubemapParams);
			bool Create(Image referenceImage, const TextureAssetParams& params = TextureAssetParams{});
			bool Create(Stream& imageStream, const TextureAssetParams& params = TextureAssetParams{});
			bool Create(Stream& imageStream,  const TextureAssetParams& params, const Vector2ui32& atlasSize);
			bool Create(Stream& imageStream,  const TextureAssetParams& params, const CubemapParams& cubemapParams);
			bool Create(const TextureInfo& textureInfo, ImageBuilder imageBuilder, const TextureAssetParams& params = TextureAssetParams{});
			bool Create(const TextureInfo& textureInfo, TextureBuilder textureBuilder, const TextureAssetParams& params = TextureAssetParams{});
			void Destroy();

			inline PixelFormat GetFormat() const;
			inline UInt8 GetLevelCount() const;
			const std::shared_ptr<Texture>& GetOrCreateTexture(RenderDevice& renderDevice) const;
			inline Vector3ui GetSize(UInt8 level = 0) const;
			inline const TextureInfo& GetTextureInfo() const;
			inline ImageType GetType() const;

			TextureAsset& operator=(const TextureAsset&) = delete;
			TextureAsset& operator=(TextureAsset&&) = default;

			static std::shared_ptr<TextureAsset> CreateFromImage(Image referenceImage, const TextureAssetParams& params = TextureAssetParams{});
			static std::shared_ptr<TextureAsset> CreateFromTexture(std::shared_ptr<Texture> texture);
			static std::shared_ptr<TextureAsset> CreateView(std::shared_ptr<TextureAsset> textureAsset, const TextureViewInfo& viewInfo, const TextureAssetParams& params = TextureAssetParams{});
			static std::shared_ptr<TextureAsset> CreateWithBuilder(const TextureInfo& textureInfo, ImageBuilder builder, const TextureAssetParams& params = TextureAssetParams{});
			static std::shared_ptr<TextureAsset> CreateWithBuilder(const TextureInfo& textureInfo, TextureBuilder builder, const TextureAssetParams& params = TextureAssetParams{});

			static std::shared_ptr<TextureAsset> OpenFromFile(const std::filesystem::path& filePath, const TextureAssetParams& params = TextureAssetParams{});
			static std::shared_ptr<TextureAsset> OpenFromMemory(const void* data, std::size_t size, const TextureAssetParams& params = TextureAssetParams{});
			static std::shared_ptr<TextureAsset> OpenFromStream(std::unique_ptr<Stream> stream, const TextureAssetParams& params = TextureAssetParams{});
			static std::shared_ptr<TextureAsset> OpenFromStream(Stream& stream, const TextureAssetParams& params = TextureAssetParams{});

			// OpenArray
			static std::shared_ptr<TextureAsset> OpenFromFile(const std::filesystem::path& filePath,  const TextureAssetParams& params, const Vector2ui32& atlasSize);
			static std::shared_ptr<TextureAsset> OpenFromMemory(const void* data, std::size_t size,  const TextureAssetParams& params, const Vector2ui32& atlasSize);
			static std::shared_ptr<TextureAsset> OpenFromStream(std::unique_ptr<Stream> stream,  const TextureAssetParams& params, const Vector2ui32& atlasSize);
			static std::shared_ptr<TextureAsset> OpenFromStream(Stream& stream,  const TextureAssetParams& params, const Vector2ui32& atlasSize);

			// OpenCubemap
			static std::shared_ptr<TextureAsset> OpenFromFile(const std::filesystem::path& filePath,  const TextureAssetParams& params, const CubemapParams& cubemapParams);
			static std::shared_ptr<TextureAsset> OpenFromMemory(const void* data, std::size_t size,  const TextureAssetParams& params, const CubemapParams& cubemapParams);
			static std::shared_ptr<TextureAsset> OpenFromStream(std::unique_ptr<Stream> stream,  const TextureAssetParams& params, const CubemapParams& cubemapParams);
			static std::shared_ptr<TextureAsset> OpenFromStream(Stream& stream,  const TextureAssetParams& params, const CubemapParams& cubemapParams);

		private:
			struct TextureEntry;

			inline TextureEntry* GetEntry(RenderDevice& device) const;
			TextureEntry* GetOrCreateEntry(RenderDevice& device) const;

			void StoreTextureInfoAndParams(const TextureInfo& textureInfo, const TextureAssetParams& params);

			struct NoParams {};
			struct NoSource {};

			struct ImageSource
			{
				Image image;
			};

			struct StreamSource
			{
				std::unique_ptr<Stream> ownedStream;
				std::variant<NoParams, Vector2ui32, CubemapParams> additionalParam;
				MovablePtr<Stream> stream;
				UInt64 originalStreamPos;
			};

			struct TextureViewSource
			{
				std::shared_ptr<TextureAsset> texture;
				TextureViewInfo viewInfo;
			};

			struct TextureEntry
			{
				std::shared_ptr<Texture> texture;
				MovablePtr<RenderDevice> device;

				NazaraSlot(RenderDevice, OnRenderDeviceRelease, onDeviceRelease);
			};

			std::variant<NoSource, ImageBuilder, ImageSource, StreamSource, TextureBuilder, TextureViewSource> m_source;
			mutable FixedVector<TextureEntry, 4> m_entries; //< handling at most 4 GPUs seems pretty reasonable
			TextureInfo m_textureInfo;
			TextureAssetParams m_params;
	};
}

#include <Nazara/Graphics/TextureAsset.inl>

#endif // NAZARA_GRAPHICS_TEXTUREASSET_HPP
