// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_TEXTUREASSET_HPP
#define NAZARA_GRAPHICS_TEXTUREASSET_HPP

#include <NazaraUtils/Prerequisites.hpp>
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
	};

	class TextureAsset;

	using TextureAssetLibrary = ObjectLibrary<TextureAsset>;
	using TextureAssetLoader = ResourceLoader<TextureAsset, TextureAssetParams>;
	using TextureAssetManager = ResourceManager<TextureAsset, TextureAssetParams>;

	class NAZARA_GRAPHICS_API TextureAsset : public Resource
	{
		public:
			using Params = TextureAssetParams;

			TextureAsset() = default;
			TextureAsset(const TextureAsset&) = delete;
			TextureAsset(TextureAsset&&) = default;
			inline ~TextureAsset();

			bool Create(Image referenceImage);
			bool Create(std::unique_ptr<Stream> imageStream);
			bool Create(Stream& imageStream);
			bool Create(std::shared_ptr<Texture> texture);
			bool Create(std::shared_ptr<TextureAsset> textureAsset, const TextureViewInfo& viewInfo);
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
			static std::shared_ptr<TextureAsset> CreateView(std::shared_ptr<TextureAsset> textureAsset, const TextureViewInfo& viewInfo);

			static std::shared_ptr<TextureAsset> OpenFromFile(const std::filesystem::path& filePath, const TextureAssetParams& params = TextureAssetParams{});
			static std::shared_ptr<TextureAsset> OpenFromMemory(const void* data, std::size_t size, const TextureAssetParams& params = TextureAssetParams{});
			static std::shared_ptr<TextureAsset> OpenFromStream(std::unique_ptr<Stream> stream, const TextureAssetParams& params = TextureAssetParams{});
			static std::shared_ptr<TextureAsset> OpenFromStream(Stream& stream, const TextureAssetParams& params = TextureAssetParams{});

		private:
			struct TextureEntry;

			inline TextureEntry* GetEntry(RenderDevice& device) const;
			TextureEntry* GetOrCreateEntry(RenderDevice& device) const;

			struct NoSource {};

			struct ImageSource
			{
				Image image;
			};

			struct StreamSource
			{
				std::unique_ptr<Stream> ownedStream;
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

			std::variant<NoSource, ImageSource, StreamSource, TextureViewSource> m_source;
			mutable FixedVector<TextureEntry, 4> m_entries; //< handling at most 4 GPUs seems pretty reasonable
			TextureInfo m_textureInfo;
	};
}

#include <Nazara/Graphics/TextureAsset.inl>

#endif // NAZARA_GRAPHICS_TEXTUREASSET_HPP
