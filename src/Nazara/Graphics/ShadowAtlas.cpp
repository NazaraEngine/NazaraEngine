// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/ShadowAtlas.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>

namespace Nz
{
	ShadowAtlas::ShadowAtlas(RenderDevice& renderDevice, UInt32 atlasSize) :
	m_atlasSize(atlasSize),
	m_renderDevice(renderDevice)
	{
		TextureInfo depthTextureParams;
		depthTextureParams.usageFlags = TextureUsage::DepthStencilAttachment | TextureUsage::ShaderSampling;
		depthTextureParams.type = ImageType::E2D;
		depthTextureParams.pixelFormat = PixelFormat::Depth16;
		depthTextureParams.width = m_atlasSize;
		depthTextureParams.height = m_atlasSize;
		depthTextureParams.levelCount = 1;

		m_atlasTexture = renderDevice.InstantiateTexture(depthTextureParams);
		m_atlasTexture->UpdateDebugName("Shadow atlas");
	}

	void ShadowAtlas::Clear()
	{
		m_entries.clear();
		m_binPacker.Reset({ m_atlasSize, m_atlasSize });
	}

	void ShadowAtlas::Pack()
	{
		for (auto& entry : m_entries)
		{
			if (!m_binPacker.Insert(&entry.rect, 1, false, GuillotineBinPack::RectBestAreaFit, GuillotineBinPack::SplitMinimizeArea))
			{
				entry.rect.x = InvalidPosition;
			}
		}
	}
}
