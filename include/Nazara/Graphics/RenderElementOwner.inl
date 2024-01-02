// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline RenderElementOwner::RenderElementOwner(RenderElementPoolBase* pool, std::size_t poolIndex, RenderElement* element) :
	m_poolIndex(poolIndex),
	m_element(element),
	m_pool(pool)
	{
	}

	inline RenderElement* RenderElementOwner::GetElement()
	{
		return m_element;
	}

	inline const RenderElement* RenderElementOwner::GetElement() const
	{
		return m_element;
	}

	inline RenderElement* RenderElementOwner::operator->()
	{
		// FIXME: Element pointer could also be retrieved from memory pool using poolIndex at the cost of a division
		return m_element;
	}

	inline const RenderElement* RenderElementOwner::operator->() const
	{
		return m_element;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
