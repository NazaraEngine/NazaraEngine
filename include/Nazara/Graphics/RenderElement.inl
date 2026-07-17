// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <NazaraUtils/Algorithm.hpp>

namespace Nz
{
	inline RenderElement::RenderElement(BasicRenderElement elementType, UInt32 instanceIndex, Int32 renderLayer, UInt32 renderMask) :
	RenderElement(SafeCast<UInt8>(elementType), instanceIndex, renderLayer, renderMask)
	{
	}

	inline RenderElement::RenderElement(UInt8 elementType, UInt32 instanceIndex, Int32 renderLayer, UInt32 renderMask) :
	m_renderLayer(renderLayer),
	m_elementType(elementType),
	m_instanceIndex(instanceIndex),
	m_renderMask(renderMask),
	m_sortKey(0)
	{
	}

	inline UInt8 RenderElement::GetElementType() const
	{
		return m_elementType;
	}

	inline UInt32 RenderElement::GetInstanceIndex() const
	{
		return m_instanceIndex;
	}

	inline Int32 RenderElement::GetRenderLayer() const
	{
		return m_renderLayer;
	}

	inline UInt32 RenderElement::GetRenderMask() const
	{
		return m_renderMask;
	}

	inline UInt64 RenderElement::GetSortKey() const
	{
		return m_sortKey;
	}

	inline void RenderElement::UpdateSortKey(const RenderQueueRegistry& registry)
	{
		m_sortKey = ComputeSortKey(registry);
	}
}
