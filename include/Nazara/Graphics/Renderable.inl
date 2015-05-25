// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

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
