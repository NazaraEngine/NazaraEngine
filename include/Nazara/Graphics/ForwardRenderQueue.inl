// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardRenderQueue.hpp>

namespace Nz
{
	inline void ForwardRenderQueue::RegisterLayer(int layerIndex)
	{
		auto it = std::lower_bound(m_renderLayers.begin(), m_renderLayers.end(), layerIndex);
		if (it == m_renderLayers.end() || *it != layerIndex)
			m_renderLayers.insert(it, layerIndex);
	}
}
