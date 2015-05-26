// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

NzRenderable::NzRenderable(const NzRenderable& renderable) :
m_boundingVolume(renderable.m_boundingVolume),
m_boundingVolumeUpdated(renderable.m_boundingVolumeUpdated)
{
}

void NzRenderable::EnsureBoundingVolumeUpdated() const
{
	if (!m_boundingVolumeUpdated)
		UpdateBoundingVolume();
}

void NzRenderable::InvalidateBoundingVolume()
{
	m_boundingVolumeUpdated = false;
}

void NzRenderable::UpdateBoundingVolume() const
{
	MakeBoundingVolume();
	m_boundingVolumeUpdated = true;
}

NzRenderable& NzRenderable::operator=(const NzRenderable& renderable)
{
	m_boundingVolume = renderable.m_boundingVolume;
	m_boundingVolumeUpdated = renderable.m_boundingVolumeUpdated;
}
