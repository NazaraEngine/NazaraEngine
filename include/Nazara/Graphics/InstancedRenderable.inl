// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

namespace Nz
{
	inline InstancedRenderable::InstancedRenderable(const InstancedRenderable& renderable) :
	RefCounted(),
	m_boundingVolume(renderable.m_boundingVolume),
	m_boundingVolumeUpdated(renderable.m_boundingVolumeUpdated)
	{
	}

	inline void InstancedRenderable::EnsureBoundingVolumeUpdated() const
	{
		if (!m_boundingVolumeUpdated)
			UpdateBoundingVolume();
	}

	inline void InstancedRenderable::InvalidateBoundingVolume()
	{
		m_boundingVolumeUpdated = false;
	}

	inline void InstancedRenderable::InvalidateInstanceData(UInt32 flags)
	{
		OnInstancedRenderableInvalidateData(this, flags);
	}

	inline InstancedRenderable& InstancedRenderable::operator=(const InstancedRenderable& renderable)
	{
		m_boundingVolume = renderable.m_boundingVolume;
		m_boundingVolumeUpdated = renderable.m_boundingVolumeUpdated;

		return *this;
	}

	inline void InstancedRenderable::UpdateBoundingVolume() const
	{
		MakeBoundingVolume();
		m_boundingVolumeUpdated = true;
	}
}
