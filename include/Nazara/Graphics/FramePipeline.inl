// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/FramePipeline.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline DebugDrawer& FramePipeline::GetDebugDrawer()
	{
		return m_debugDrawer;
	}

	inline ElementRenderer& FramePipeline::GetElementRenderer(std::size_t elementIndex)
	{
		assert(elementIndex < m_elementRenderers.size());
		return *m_elementRenderers[elementIndex];
	}

	inline std::size_t FramePipeline::GetElementRendererCount() const
	{
		return m_elementRenderers.size();
	}

	template<typename F>
	void FramePipeline::ForEachElementRenderer(F&& callback)
	{
		for (std::size_t i = 0; i < m_elementRenderers.size(); ++i)
		{
			if (m_elementRenderers[i])
				callback(i, *m_elementRenderers[i]);
		}
	}

	template<typename F>
	void FramePipeline::ProcessRenderQueue(const RenderQueue<RenderElement*>& renderQueue, F&& callback)
	{
		if (renderQueue.empty())
			return;

		auto it = renderQueue.begin();
		auto itEnd = renderQueue.end();
		while (it != itEnd)
		{
			const RenderElement* element = *it;
			UInt8 elementType = element->GetElementType();

			const Pointer<RenderElement>* first = it;

			++it;
			while (it != itEnd && (*it)->GetElementType() == elementType)
				++it;

			std::size_t count = it - first;
			if (elementType >= m_elementRenderers.size() || !m_elementRenderers[elementType])
				continue;

			callback(elementType, first, count);
		}
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
