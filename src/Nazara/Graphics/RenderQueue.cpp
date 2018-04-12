// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Core/TaskScheduler.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	void RenderQueueInternal::Sort()
	{
		std::sort(m_orderedRenderQueue.begin(), m_orderedRenderQueue.end(), [](const RenderDataPair& lhs, const RenderDataPair& rhs)
		{
			return lhs.first < rhs.first;
		});
	}
}
