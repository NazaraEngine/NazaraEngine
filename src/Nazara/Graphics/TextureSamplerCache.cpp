// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/TextureSamplerCache.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>

namespace Nz
{
	const std::shared_ptr<TextureSampler>& TextureSamplerCache::Get(const TextureSamplerInfo& info)
	{
		auto it = m_samplers.find(info);
		if (it == m_samplers.end())
			it = m_samplers.emplace(info, m_device->InstantiateTextureSampler(info)).first;

		return it->second;
	}
}
