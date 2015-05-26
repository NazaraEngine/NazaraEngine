// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Renderable.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzRenderable::~NzRenderable() = default;

bool NzRenderable::Cull(const NzFrustumf& frustum, const NzBoundingVolumef& volume, const NzMatrix4f& transformMatrix) const
{
	NazaraUnused(transformMatrix);

	return frustum.Contains(volume);
}

const NzBoundingVolumef& NzRenderable::GetBoundingVolume() const
{
	EnsureBoundingVolumeUpdated();

	return m_boundingVolume;
}

void NzRenderable::UpdateBoundingVolume(NzBoundingVolumef* boundingVolume, const NzMatrix4f& transformMatrix) const
{
	NazaraAssert(boundingVolume, "Invalid bounding volume");

	boundingVolume->Update(transformMatrix);
}

NzRenderableLibrary::LibraryMap NzRenderable::s_library;
