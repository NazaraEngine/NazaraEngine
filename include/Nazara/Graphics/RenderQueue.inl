#include "RenderQueue.hpp"
// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline RenderQueue::RenderQueue(std::size_t passIndex, RenderQueueFlags flags) :
	m_passIndex(passIndex),
	m_flags(flags),
	m_shouldRebuildRenderQueue(false),
	m_shouldSortRenderQueue(false)
	{
	}

	inline void RenderQueue::BeginRegisterRenderable()
	{
		m_firstAddedElementIndex = m_renderElements.size();
	}

	inline std::size_t RenderQueue::GetContentHash() const
	{
		return m_contentHash;
	}

	inline RenderQueueFlags RenderQueue::GetFlags() const
	{
		return m_flags;
	}

	inline std::size_t RenderQueue::GetPassIndex() const
	{
		return m_passIndex;
	}

	template<typename F>
	void RenderQueue::Process(UInt32 renderMask, F&& callback) const
	{
		auto it = m_orderedRenderElements.data();
		auto itEnd = it + m_orderedRenderElements.size();
		while (it != itEnd)
		{
			const RenderElement* element = *it;
			if ((element->GetRenderMask() & renderMask) == 0)
			{
				++it;
				continue;
			}

			UInt8 elementType = element->GetElementType();

			const Pointer<const RenderElement>* first = it;

			++it;
			while (it != itEnd && (*it)->GetElementType() == elementType && ((*it)->GetRenderMask() & renderMask))
				++it;

			std::size_t count = it - first;
			callback(elementType, first, count);
		}
	}

	template<typename F>
	void RenderQueue::RegisterRenderable(F&& callback)
	{
		callback(m_renderElements);
	}

	template<typename F>
	void RenderQueue::SortRenderQueue(F&& callback)
	{
		callback(m_orderedRenderElements);
		RecomputeContentHash();
		m_shouldSortRenderQueue = false;
	}
}
