// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline RenderTarget::RenderTarget(Int32 renderOrder) :
	m_renderOrder(renderOrder)
	{
	}

	inline Int32 RenderTarget::GetRenderOrder() const
	{
		return m_renderOrder;
	}

	inline void RenderTarget::UpdateRenderOrder(Int32 renderOrder)
	{
		OnRenderTargetRenderOrderChange(this, renderOrder);
		m_renderOrder = renderOrder;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
