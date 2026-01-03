// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/TextureAsset.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <NazaraUtils/PathUtils.hpp>
#include <algorithm>
#include <cassert>

namespace Nz
{
	bool TextureAssetParams::IsValid() const
	{
		return true;
	}

	void TextureAssetParams::Merge(const TextureAssetParams& /*params*/)
	{
	}

	bool TextureAsset::Create(std::shared_ptr<Texture> texture)
	{
		NazaraAssertMsg(texture, "invalid texture");

		Destroy();

		m_textureInfo = texture->GetTextureInfo();
		m_params = {};
		m_params.usageFlags = m_textureInfo.usageFlags;

		auto* entry = GetOrCreateEntry(*texture->GetDevice());
		assert(entry);

		entry->texture = std::move(texture);

		return true;
	}

	bool TextureAsset::Create(std::shared_ptr<TextureAsset> textureAsset, const TextureViewInfo& viewInfo, const TextureAssetParams& params)
	{
		NazaraAssertMsg(textureAsset, "invalid texture asset");

		Destroy();

		auto& viewSource = m_source.emplace<TextureViewSource>();
		viewSource.texture = std::move(textureAsset);
		viewSource.viewInfo = viewInfo;

		m_textureInfo = Texture::ApplyView(viewSource.texture->GetTextureInfo(), viewSource.viewInfo);
		m_params = {};
		m_params.usageFlags = m_textureInfo.usageFlags;

		return true;
	}

	bool TextureAsset::Create(std::unique_ptr<Stream> imageStream, const TextureAssetParams& params)
	{
		NazaraAssertMsg(imageStream, "invalid image stream");

		Destroy();

		auto& streamSource = m_source.emplace<StreamSource>();
		streamSource.originalStreamPos = imageStream->GetCursorPos();

		ImageParams imageParams;
		imageParams.levels.reset(); // don't load any mipmap

		std::shared_ptr<Image> image = Image::LoadFromStream(*imageStream, imageParams);
		if (!image)
		{
			if (!imageStream->GetPath().empty())
				NazaraError("failed to load image from file {}", PathToString(imageStream->GetPath()));
			else
				NazaraError("failed to load image from stream");

			return false;
		}

		streamSource.ownedStream = std::move(imageStream);
		streamSource.stream = streamSource.ownedStream.get();
		StoreTextureInfoAndParams(Texture::BuildTextureInfo(*image), params);

		return true;
	}

	bool TextureAsset::Create(std::unique_ptr<Stream> imageStream, const TextureAssetParams& params, const Vector2ui32& atlasSize)
	{
		NazaraAssertMsg(imageStream, "invalid image stream");

		Destroy();

		auto& streamSource = m_source.emplace<StreamSource>();
		streamSource.originalStreamPos = imageStream->GetCursorPos();

		ImageParams imageParams;
		imageParams.levels.reset(); // don't load any mipmap

		std::shared_ptr<Image> image = Image::LoadFromStream(*imageStream, imageParams, atlasSize);
		if (!image)
		{
			if (!imageStream->GetPath().empty())
				NazaraError("failed to load image from file {}", PathToString(imageStream->GetPath()));
			else
				NazaraError("failed to load image from stream");

			return false;
		}

		streamSource.additionalParam = atlasSize;
		streamSource.ownedStream = std::move(imageStream);
		streamSource.stream = streamSource.ownedStream.get();
		StoreTextureInfoAndParams(Texture::BuildTextureInfo(*image), params);

		return true;
	}

	bool TextureAsset::Create(std::unique_ptr<Stream> imageStream, const TextureAssetParams& params, const CubemapParams& cubemapParams)
	{
		NazaraAssertMsg(imageStream, "invalid image stream");

		Destroy();

		auto& streamSource = m_source.emplace<StreamSource>();
		streamSource.originalStreamPos = imageStream->GetCursorPos();

		ImageParams imageParams;
		imageParams.levels.reset(); // don't load any mipmap

		std::shared_ptr<Image> image = Image::LoadFromStream(*imageStream, imageParams, cubemapParams);
		if (!image)
		{
			if (!imageStream->GetPath().empty())
				NazaraError("failed to load image from file {}", PathToString(imageStream->GetPath()));
			else
				NazaraError("failed to load image from stream");

			return false;
		}

		streamSource.additionalParam = cubemapParams;
		streamSource.ownedStream = std::move(imageStream);
		streamSource.stream = streamSource.ownedStream.get();
		StoreTextureInfoAndParams(Texture::BuildTextureInfo(*image), params);

		return true;
	}

	bool TextureAsset::Create(Image referenceImage, const TextureAssetParams& params)
	{
		NazaraAssertMsg(referenceImage.IsValid(), "invalid image");

		Destroy();

		auto& imageSource = m_source.emplace<ImageSource>();
		imageSource.image = std::move(referenceImage);
		StoreTextureInfoAndParams(Texture::BuildTextureInfo(imageSource.image), params);

		return true;
	}

	bool TextureAsset::Create(Stream& imageStream, const TextureAssetParams& params)
	{
		Destroy();

		auto& streamSource = m_source.emplace<StreamSource>();
		streamSource.originalStreamPos = imageStream.GetCursorPos();

		ImageParams imageParams;
		imageParams.levels.reset(); // don't load any mipmap

		std::shared_ptr<Image> image = Image::LoadFromStream(imageStream, imageParams);
		if (!image)
		{
			if (!imageStream.GetPath().empty())
				NazaraError("failed to load image from file {}", PathToString(imageStream.GetPath()));
			else
				NazaraError("failed to load image from stream");

			return false;
		}

		streamSource.stream = &imageStream;
		StoreTextureInfoAndParams(Texture::BuildTextureInfo(*image), params);

		return true;
	}

	bool TextureAsset::Create(Stream& imageStream, const TextureAssetParams& params, const Vector2ui32& atlasSize)
	{
		Destroy();

		auto& streamSource = m_source.emplace<StreamSource>();
		streamSource.originalStreamPos = imageStream.GetCursorPos();

		ImageParams imageParams;
		imageParams.levels.reset(); // don't load any mipmap

		std::shared_ptr<Image> image = Image::LoadFromStream(imageStream, imageParams, atlasSize);
		if (!image)
		{
			if (!imageStream.GetPath().empty())
				NazaraError("failed to load image from file {}", PathToString(imageStream.GetPath()));
			else
				NazaraError("failed to load image from stream");

			return false;
		}

		streamSource.additionalParam = atlasSize;
		streamSource.stream = &imageStream;
		StoreTextureInfoAndParams(Texture::BuildTextureInfo(*image), params);

		return true;
	}

	bool TextureAsset::Create(Stream& imageStream, const TextureAssetParams& params, const CubemapParams& cubemapParams)
	{
		Destroy();

		auto& streamSource = m_source.emplace<StreamSource>();
		streamSource.originalStreamPos = imageStream.GetCursorPos();

		ImageParams imageParams;
		imageParams.levels.reset(); // don't load any mipmap

		std::shared_ptr<Image> image = Image::LoadFromStream(imageStream, imageParams, cubemapParams);
		if (!image)
		{
			if (!imageStream.GetPath().empty())
				NazaraError("failed to load image from file {}", PathToString(imageStream.GetPath()));
			else
				NazaraError("failed to load image from stream");

			return false;
		}

		streamSource.additionalParam = cubemapParams;
		streamSource.stream = &imageStream;
		StoreTextureInfoAndParams(Texture::BuildTextureInfo(*image), params);

		return true;
	}

	bool TextureAsset::Create(const TextureInfo& textureInfo, ImageBuilder imageBuilder, const TextureAssetParams& params)
	{
		NazaraAssertMsg(imageBuilder, "invalid builder");

		Destroy();

		m_source = std::move(imageBuilder);
		StoreTextureInfoAndParams(textureInfo, params);

		return true;
	}

	bool TextureAsset::Create(const TextureInfo& textureInfo, TextureBuilder textureBuilder, const TextureAssetParams& params)
	{
		NazaraAssertMsg(textureBuilder, "invalid builder");

		Destroy();

		m_source = std::move(textureBuilder);
		StoreTextureInfoAndParams(textureInfo, params);

		return true;
	}

	void TextureAsset::Destroy()
	{
		m_entries.clear();
		m_source = NoSource{};
	}

	const std::shared_ptr<Texture>& TextureAsset::GetOrCreateTexture(RenderDevice& renderDevice) const
	{
		TextureEntry* entry = GetOrCreateEntry(renderDevice);
		if NAZARA_UNLIKELY(!entry)
		{
			static std::shared_ptr<Texture> emptyTexture;
			return emptyTexture;
		}

		if (!entry->texture)
		{
			std::visit(Overloaded {
				[](NoSource)
				{
					NazaraError("can't create texture as no source has been defined");
				},
				[&](const ImageBuilder& imageBuilder)
				{
					Image image = imageBuilder(renderDevice, m_params);

					entry->texture = renderDevice.InstantiateTexture(m_textureInfo, image.GetConstPixels(), true);
				},
				[&](const ImageSource& imageSource)
				{
					entry->texture = renderDevice.InstantiateTexture(m_textureInfo, imageSource.image.GetConstPixels(), true);
				},
				[&](const StreamSource& streamSource)
				{
					streamSource.stream->SetCursorPos(streamSource.originalStreamPos);

					std::shared_ptr<Image> image;
					std::visit([&](const auto& arg)
					{
						using T = std::decay_t<decltype(arg)>;

						if constexpr (std::is_same_v<T, NoParams>)
							image = Image::LoadFromStream(*streamSource.stream);
						else
						{
							ImageParams defaultParams;
							image = Image::LoadFromStream(*streamSource.stream, defaultParams, arg);
						}
					}, streamSource.additionalParam);

					if (image)
						entry->texture = renderDevice.InstantiateTexture(m_textureInfo, image->GetConstPixels(), true);
					else
						NazaraError("failed to load image from stream {}", streamSource.stream->GetPath());
				},
				[&](const TextureBuilder& textureBuilder)
				{
					entry->texture = textureBuilder(renderDevice, m_params);
				},
				[&](const TextureViewSource& viewSource)
				{
					const std::shared_ptr<Texture>& texture = viewSource.texture->GetOrCreateTexture(renderDevice);
					entry->texture = texture->CreateView(viewSource.viewInfo);
				}
			}, m_source);
		}

		return entry->texture;
	}

	std::shared_ptr<TextureAsset> TextureAsset::CreateFromImage(Image referenceImage, const TextureAssetParams& params)
	{
		std::shared_ptr<TextureAsset> texAsset = std::make_shared<TextureAsset>();
		if (!texAsset->Create(std::move(referenceImage), params))
			return {};

		return texAsset;
	}

	std::shared_ptr<TextureAsset> TextureAsset::CreateFromTexture(std::shared_ptr<Texture> texture)
	{
		std::shared_ptr<TextureAsset> texAsset = std::make_shared<TextureAsset>();
		if (!texAsset->Create(std::move(texture)))
			return {};

		return texAsset;
	}

	std::shared_ptr<TextureAsset> TextureAsset::CreateView(std::shared_ptr<TextureAsset> textureAsset, const TextureViewInfo& viewInfo, const TextureAssetParams& params)
	{
		std::shared_ptr<TextureAsset> texAsset = std::make_shared<TextureAsset>();
		if (!texAsset->Create(std::move(textureAsset), viewInfo, params))
			return {};

		return texAsset;
	}

	std::shared_ptr<TextureAsset> TextureAsset::CreateWithBuilder(const TextureInfo& textureInfo, ImageBuilder builder, const TextureAssetParams& params)
	{
		std::shared_ptr<TextureAsset> texAsset = std::make_shared<TextureAsset>();
		if (!texAsset->Create(textureInfo, std::move(builder), params))
			return {};

		return texAsset;
	}

	std::shared_ptr<TextureAsset> TextureAsset::CreateWithBuilder(const TextureInfo& textureInfo, TextureBuilder builder, const TextureAssetParams& params)
	{
		std::shared_ptr<TextureAsset> texAsset = std::make_shared<TextureAsset>();
		if (!texAsset->Create(textureInfo, std::move(builder), params))
			return {};

		return texAsset;
	}

	std::shared_ptr<TextureAsset> TextureAsset::OpenFromFile(const std::filesystem::path& filePath, const TextureAssetParams& params)
	{
		std::unique_ptr<Nz::File> file = std::make_unique<Nz::File>(filePath, Nz::OpenMode::Read);
		if (!file->IsOpen())
		{
			NazaraError("failed to open {}", filePath);
			return {};
		}

		return OpenFromStream(std::move(file), params);
	}

	std::shared_ptr<TextureAsset> TextureAsset::OpenFromMemory(const void* data, std::size_t size, const TextureAssetParams& params)
	{
		return OpenFromStream(std::make_unique<Nz::MemoryView>(data, size), params);
	}

	std::shared_ptr<TextureAsset> TextureAsset::OpenFromStream(std::unique_ptr<Stream> stream, const TextureAssetParams& params)
	{
		std::shared_ptr<TextureAsset> texAsset = std::make_shared<TextureAsset>();
		if (!texAsset->Create(std::move(stream), params))
			return {};

		return texAsset;
	}

	std::shared_ptr<TextureAsset> TextureAsset::OpenFromStream(Stream& stream, const TextureAssetParams& params)
	{
		std::shared_ptr<TextureAsset> texAsset = std::make_shared<TextureAsset>();
		if (!texAsset->Create(stream, params))
			return {};

		return texAsset;
	}

	std::shared_ptr<TextureAsset> TextureAsset::OpenFromFile(const std::filesystem::path& filePath, const TextureAssetParams& params, const Vector2ui32& atlasSize)
	{
		std::unique_ptr<Nz::File> file = std::make_unique<Nz::File>(filePath, Nz::OpenMode::Read);
		if (!file->IsOpen())
		{
			NazaraError("failed to open {}", filePath);
			return {};
		}

		return OpenFromStream(std::move(file), params, atlasSize);
	}

	std::shared_ptr<TextureAsset> TextureAsset::OpenFromMemory(const void* data, std::size_t size, const TextureAssetParams& params, const Vector2ui32& atlasSize)
	{
		return OpenFromStream(std::make_unique<Nz::MemoryView>(data, size), params, atlasSize);
	}

	std::shared_ptr<TextureAsset> TextureAsset::OpenFromStream(std::unique_ptr<Stream> stream, const TextureAssetParams& params, const Vector2ui32& atlasSize)
	{
		std::shared_ptr<TextureAsset> texAsset = std::make_shared<TextureAsset>();
		if (!texAsset->Create(std::move(stream), params, atlasSize))
			return {};

		return texAsset;
	}

	std::shared_ptr<TextureAsset> TextureAsset::OpenFromStream(Stream& stream, const TextureAssetParams& params, const Vector2ui32& atlasSize)
	{
		std::shared_ptr<TextureAsset> texAsset = std::make_shared<TextureAsset>();
		if (!texAsset->Create(stream, params, atlasSize))
			return {};

		return texAsset;
	}

	std::shared_ptr<TextureAsset> TextureAsset::OpenFromFile(const std::filesystem::path& filePath, const TextureAssetParams& params, const CubemapParams& cubemapParams)
	{
		std::unique_ptr<Nz::File> file = std::make_unique<Nz::File>(filePath, Nz::OpenMode::Read);
		if (!file->IsOpen())
		{
			NazaraError("failed to open {}", filePath);
			return {};
		}

		return OpenFromStream(std::move(file), params, cubemapParams);
	}

	std::shared_ptr<TextureAsset> TextureAsset::OpenFromMemory(const void* data, std::size_t size, const TextureAssetParams& params, const CubemapParams& cubemapParams)
	{
		return OpenFromStream(std::make_unique<Nz::MemoryView>(data, size), params, cubemapParams);
	}

	std::shared_ptr<TextureAsset> TextureAsset::OpenFromStream(std::unique_ptr<Stream> stream, const TextureAssetParams& params, const CubemapParams& cubemapParams)
	{
		std::shared_ptr<TextureAsset> texAsset = std::make_shared<TextureAsset>();
		if (!texAsset->Create(std::move(stream), params, cubemapParams))
			return {};

		return texAsset;
	}

	std::shared_ptr<TextureAsset> TextureAsset::OpenFromStream(Stream& stream, const TextureAssetParams& params, const CubemapParams& cubemapParams)
	{
		std::shared_ptr<TextureAsset> texAsset = std::make_shared<TextureAsset>();
		if (!texAsset->Create(stream, params, cubemapParams))
			return {};

		return texAsset;
	}

	auto TextureAsset::GetOrCreateEntry(RenderDevice& device) const -> TextureEntry*
	{
		TextureEntry* entry = GetEntry(device);
		if (!entry)
		{
			// We can't create this texture on another device if we don't have a source
			if (!m_entries.empty() && std::holds_alternative<NoSource>(m_source))
				return nullptr;

			entry = &m_entries.emplace_back();
			entry->device = &device;
			entry->onDeviceRelease.Connect(device.OnRenderDeviceRelease, [this](RenderDevice* device)
			{
				// Destroy texture before the device is released and free the slot
				auto it = std::find_if(m_entries.begin(), m_entries.end(), [device](const TextureEntry& entry) { return entry.device == device; });
				assert(it != m_entries.end());
				m_entries.erase(it);
			});
		}

		return entry;
	}

	void TextureAsset::StoreTextureInfoAndParams(const TextureInfo& textureInfo, const TextureAssetParams& params)
	{
		m_textureInfo = textureInfo;
		m_params = params;

		// apply params
		m_textureInfo.usageFlags = params.usageFlags;
		if (params.sRGB)
			m_textureInfo.pixelFormat = PixelFormatInfo::ToSRGB(m_textureInfo.pixelFormat).value_or(m_textureInfo.pixelFormat);
	}
}
