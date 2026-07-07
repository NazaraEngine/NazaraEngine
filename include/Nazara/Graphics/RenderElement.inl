// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <NazaraUtils/Algorithm.hpp>

namespace Nz
{
	inline RenderElement::RenderElement(BasicRenderElement elementType, UInt32 renderMask) :
	RenderElement(SafeCast<UInt8>(elementType), renderMask)
	{
	}

	inline RenderElement::RenderElement(UInt8 elementType, UInt32 renderMask) :
	m_elementType(elementType),
	m_renderMask(renderMask)
	{
	}

	inline UInt8 RenderElement::GetElementType() const
	{
		return m_elementType;
	}

	inline UInt32 RenderElement::GetRenderMask() const
	{
		return m_renderMask;
	}
}
