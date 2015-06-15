// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Renderable.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzRenderable::~NzRenderable() = default;

bool NzRenderable::Cull(const NzFrustumf& frustum, const NzMatrix4f& transformMatrix) const
{
	NazaraUnused(transformMatrix);

	return frustum.Contains(m_boundingVolume);
}

const NzBoundingVolumef& NzRenderable::GetBoundingVolume() const
{
	EnsureBoundingVolumeUpdated();

	return m_boundingVolume;
}

void NzRenderable::UpdateBoundingVolume(const NzMatrix4f& transformMatrix)
{
	m_boundingVolume.Update(transformMatrix);
}
