// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
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

	bool TextureAsset::Create(Image referenceImage)
	{
		NazaraAssert(referenceImage.IsValid(), "invalid image");

		Destroy();

		m_image = std::move(referenceImage);
		m_textureInfo = Texture::BuildTextureInfo(m_image);

		return true;
	}

	bool TextureAsset::Create(std::unique_ptr<Stream> imageStream)
	{
		NazaraAssert(imageStream, "invalid image stream");

		Destroy();

		m_originalStreamPos = imageStream->GetCursorPos();

		ImageParams imageParams;
		imageParams.levels.reset(); // don't load any mipmap

		std::shared_ptr<Image> image = Image::LoadFromStream(*imageStream, imageParams);
		if (!image)
		{
			if (!m_stream->GetPath().empty())
				NazaraErrorFmt("failed to load image from file {}", PathToString(m_stream->GetPath()));
			else
				NazaraError("failed to load image from stream");

			return false;
		}

		m_ownedStream = std::move(imageStream);
		m_stream = m_ownedStream.get();
		m_textureInfo = Texture::BuildTextureInfo(*image);

		return true;
	}

	bool TextureAsset::Create(Stream& imageStream)
	{
		Destroy();

		m_originalStreamPos = imageStream.GetCursorPos();

		ImageParams imageParams;
		imageParams.levels.reset(); // don't load any mipmap

		std::shared_ptr<Image> image = Image::LoadFromStream(imageStream, imageParams);
		if (!image)
		{
			if (!m_stream->GetPath().empty())
				NazaraErrorFmt("failed to load image from file {}", PathToString(m_stream->GetPath()));
			else
				NazaraError("failed to load image from stream");

			return false;
		}

		m_stream = &imageStream;
		m_textureInfo = Texture::BuildTextureInfo(*image);

		return true;
	}

	bool TextureAsset::Create(std::shared_ptr<Texture> texture)
	{
		NazaraAssert(texture, "invalid texture");

		Destroy();

		m_textureInfo = texture->GetTextureInfo();

		auto* entry = GetOrCreateEntry(*texture->GetDevice());
		assert(entry);

		entry->texture = std::move(texture);

		return true;
	}

	void TextureAsset::Destroy()
	{
		m_entries.clear();
		m_image.Destroy();
		m_ownedStream.reset();
		m_stream = nullptr;
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
			if (m_image.IsValid())
			{
				entry->texture = renderDevice.InstantiateTexture(m_textureInfo, m_image.GetConstPixels(), true);
				entry->texture->SetFilePath(m_image.GetFilePath());
			}
			else
			{
				m_stream->SetCursorPos(m_originalStreamPos);

				std::shared_ptr<Image> image = Image::LoadFromStream(*m_stream);
				if (image)
				{
					entry->texture = renderDevice.InstantiateTexture(m_textureInfo, image->GetConstPixels(), true);
					entry->texture->SetFilePath(m_stream->GetPath());
				}
				else
					NazaraErrorFmt("failed to load image from stream {}", m_stream->GetPath());
			}
		}

		return entry->texture;
	}

	std::shared_ptr<TextureAsset> TextureAsset::CreateFromImage(Image referenceImage, const TextureAssetParams& params)
	{
		std::shared_ptr<TextureAsset> texAsset = std::make_shared<TextureAsset>();
		if (!texAsset->Create(std::move(referenceImage)))
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

	std::shared_ptr<TextureAsset> TextureAsset::OpenFromFile(const std::filesystem::path& filePath, const TextureAssetParams& params)
	{
		return OpenFromStream(std::make_unique<Nz::File>(filePath, Nz::OpenMode::Read), params);
	}

	std::shared_ptr<TextureAsset> TextureAsset::OpenFromMemory(const void* data, std::size_t size, const TextureAssetParams& params)
	{
		return OpenFromStream(std::make_unique<Nz::MemoryView>(data, size), params);
	}

	std::shared_ptr<TextureAsset> TextureAsset::OpenFromStream(std::unique_ptr<Stream> stream, const TextureAssetParams& params)
	{
		std::shared_ptr<TextureAsset> texAsset = std::make_shared<TextureAsset>();
		if (!texAsset->Create(std::move(stream)))
			return {};

		return texAsset;
	}

	std::shared_ptr<TextureAsset> TextureAsset::OpenFromStream(Stream& stream, const TextureAssetParams& params)
	{
		std::shared_ptr<TextureAsset> texAsset = std::make_shared<TextureAsset>();
		if (!texAsset->Create(stream))
			return {};

		return texAsset;
	}

	auto TextureAsset::GetOrCreateEntry(RenderDevice& device) const -> TextureEntry*
	{
		TextureEntry* entry = GetEntry(device);
		if (!entry)
		{
			// We can't create this texture on another device if we don't have a stream or image
			if (!m_entries.empty() && !m_image.IsValid() && !m_stream)
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
}
