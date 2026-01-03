// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline InstancedRenderable::InstancedRenderable() :
	m_aabb(Boxf::Zero()),
	m_renderLayer(0)
	{
	}

	inline const Boxf& InstancedRenderable::GetAABB() const
	{
		return m_aabb;
	}

	inline int InstancedRenderable::GetRenderLayer() const
	{
		return m_renderLayer;
	}

	inline void InstancedRenderable::UpdateRenderLayer(int renderLayer)
	{
		if (m_renderLayer != renderLayer)
		{
			m_renderLayer = renderLayer;
			OnElementInvalidated(this);
		}
	}

	inline void InstancedRenderable::UpdateAABB(Boxf aabb)
	{
		OnAABBUpdate(this, aabb);
		m_aabb = aabb;
	}
}
