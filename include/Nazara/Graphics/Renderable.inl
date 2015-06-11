// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

inline NzRenderable::NzRenderable(const NzRenderable& renderable) :
m_boundingVolume(renderable.m_boundingVolume),
m_boundingVolumeUpdated(renderable.m_boundingVolumeUpdated)
{
}

inline void NzRenderable::EnsureBoundingVolumeUpdated() const
{
	if (!m_boundingVolumeUpdated)
		UpdateBoundingVolume();
}

inline void NzRenderable::InvalidateBoundingVolume()
{
	m_boundingVolumeUpdated = false;
}

inline void NzRenderable::InvalidateInstanceData(nzUInt32 flags)
{
	OnRenderableInvalidateInstanceData(this, flags);
}

inline NzRenderable& NzRenderable::operator=(const NzRenderable& renderable)
{
	m_boundingVolume = renderable.m_boundingVolume;
	m_boundingVolumeUpdated = renderable.m_boundingVolumeUpdated;

	return *this;
}

inline void NzRenderable::UpdateBoundingVolume() const
{
	MakeBoundingVolume();
	m_boundingVolumeUpdated = true;
}
