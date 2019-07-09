// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/BasicRenderQueue.hpp>
#include <cassert>

namespace Nz
{
	inline const BasicRenderQueue::BillboardData* BasicRenderQueue::GetBillboardData(std::size_t billboardIndex) const
	{
		assert(billboardIndex < m_billboards.size());
		return &m_billboards[billboardIndex];
	}

	inline Color BasicRenderQueue::ComputeColor(float alpha)
	{
		return Color(255, 255, 255, static_cast<UInt8>(255.f * alpha));
	}

	inline Vector2f BasicRenderQueue::ComputeSinCos(float angle)
	{
		float radians = ToRadians(angle);
		return { std::sin(radians), std::cos(radians) };
	}

	inline Vector2f BasicRenderQueue::ComputeSize(float size)
	{
		return Vector2f(size, size);
	}

	inline void BasicRenderQueue::RegisterLayer(int layerIndex)
	{
		auto it = std::lower_bound(m_renderLayers.begin(), m_renderLayers.end(), layerIndex);
		if (it == m_renderLayers.end() || *it != layerIndex)
			m_renderLayers.insert(it, layerIndex);
	}
}
