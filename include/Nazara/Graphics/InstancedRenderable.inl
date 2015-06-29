// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

inline NzInstancedRenderable::NzInstancedRenderable(const NzInstancedRenderable& renderable) :
NzRefCounted(),
m_boundingVolume(renderable.m_boundingVolume),
m_boundingVolumeUpdated(renderable.m_boundingVolumeUpdated)
{
}

inline void NzInstancedRenderable::EnsureBoundingVolumeUpdated() const
{
	if (!m_boundingVolumeUpdated)
		UpdateBoundingVolume();
}

inline void NzInstancedRenderable::InvalidateBoundingVolume()
{
	m_boundingVolumeUpdated = false;
}

inline void NzInstancedRenderable::InvalidateInstanceData(nzUInt32 flags)
{
	OnInstancedRenderableInvalidateData(this, flags);
}

inline NzInstancedRenderable& NzInstancedRenderable::operator=(const NzInstancedRenderable& renderable)
{
	m_boundingVolume = renderable.m_boundingVolume;
	m_boundingVolumeUpdated = renderable.m_boundingVolumeUpdated;

	return *this;
}

inline void NzInstancedRenderable::UpdateBoundingVolume() const
{
	MakeBoundingVolume();
	m_boundingVolumeUpdated = true;
}
