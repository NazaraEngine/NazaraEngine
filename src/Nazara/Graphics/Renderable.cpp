// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Renderable.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzRenderable::~NzRenderable()
{
	OnRenderableRelease(this);
}

bool NzRenderable::Cull(const NzFrustumf& frustum, const InstanceData& instanceData) const
{
	return frustum.Contains(instanceData.volume);
}

const NzBoundingVolumef& NzRenderable::GetBoundingVolume() const
{
	EnsureBoundingVolumeUpdated();

	return m_boundingVolume;
}

void NzRenderable::InvalidateData(InstanceData* instanceData, nzUInt32 flags) const
{
	instanceData->flags |= flags;
}

void NzRenderable::UpdateBoundingVolume(InstanceData* instanceData) const
{
	NazaraAssert(instanceData, "Invalid instance data");
	NazaraUnused(instanceData);

	instanceData->volume.Update(instanceData->transformMatrix);
}

void NzRenderable::UpdateData(InstanceData* instanceData) const
{
	NazaraAssert(instanceData, "Invalid instance data");
}

NzRenderableLibrary::LibraryMap NzRenderable::s_library;
