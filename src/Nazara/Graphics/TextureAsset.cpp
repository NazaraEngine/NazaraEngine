// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/TextureAsset.hpp>
#include <algorithm>
#include <cassert>

namespace Nz
{
	bool TextureAsset::Create(Image referenceImage)
	{
		Destroy();

		m_image = std::move(referenceImage);
		return true;
	}

	bool TextureAsset::Create(std::unique_ptr<Stream> imageStream)
	{
		Destroy();

		m_ownedStream = std::move(imageStream);
		m_stream = m_ownedStream.get();
		return true;
	}

	bool TextureAsset::Create(Stream& imageStream)
	{
		Destroy();

		m_stream = &imageStream;
		return true;
	}

	bool TextureAsset::Create(std::shared_ptr<Texture> texture)
	{
		Destroy();

		auto& entry = EnsureEntry(*texture->GetDevice());
		entry.texture = std::move(texture);

		return true;
	}

	void TextureAsset::Destroy()
	{
		m_entries.clear();
		m_image.Destroy();
		m_ownedStream.reset();
		m_stream = nullptr;
	}

	auto TextureAsset::EnsureEntry(RenderDevice& device) -> TextureEntry&
	{
		TextureEntry* entry = GetEntry(device);
		if (!entry)
		{
			entry = &m_entries.emplace_back();
			entry->onDeviceRelease.Connect(device.OnRenderDeviceRelease, [this](RenderDevice* device)
			{
				// Destroy texture before the device is released and free the slot
				auto it = std::find_if(m_entries.begin(), m_entries.end(), [device](const TextureEntry& entry) { return entry.device == device; });
				assert(it != m_entries.end());
				m_entries.erase(it);
			});
		}

		return *entry;
	}

}
