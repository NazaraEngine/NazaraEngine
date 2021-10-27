// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline RenderElement::RenderElement(BasicRenderElement elementType) :
	RenderElement(static_cast<UInt8>(elementType))
	{
	}

	inline RenderElement::RenderElement(UInt8 elementType) :
	m_elementType(elementType)
	{
	}

	inline UInt8 RenderElement::GetElementType() const
	{
		return m_elementType;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
