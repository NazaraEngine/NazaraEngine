// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline RenderTarget::RenderTarget(Int32 renderOrder) :
	m_renderOrder(renderOrder),
	m_frameGraphOutput(false)
	{
	}

	inline Int32 RenderTarget::GetRenderOrder() const
	{
		return m_renderOrder;
	}

	inline bool RenderTarget::IsFrameGraphOutput() const
	{
		return m_frameGraphOutput;
	}

	inline void RenderTarget::SetFrameGraphOutput(bool output)
	{
		m_frameGraphOutput = output;
	}

	inline void RenderTarget::UpdateRenderOrder(Int32 renderOrder)
	{
		OnRenderTargetRenderOrderChange(this, renderOrder);
		m_renderOrder = renderOrder;
	}
}
