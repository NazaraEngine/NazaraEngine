// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzInstancedRenderable::~NzInstancedRenderable()
{
	OnInstancedRenderableRelease(this);
}

bool NzInstancedRenderable::Cull(const NzFrustumf& frustum, const InstanceData& instanceData) const
{
	return frustum.Contains(instanceData.volume);
}

const NzBoundingVolumef& NzInstancedRenderable::GetBoundingVolume() const
{
	EnsureBoundingVolumeUpdated();

	return m_boundingVolume;
}

void NzInstancedRenderable::InvalidateData(InstanceData* instanceData, nzUInt32 flags) const
{
	instanceData->flags |= flags;
}

void NzInstancedRenderable::UpdateBoundingVolume(InstanceData* instanceData) const
{
	NazaraAssert(instanceData, "Invalid instance data");
	NazaraUnused(instanceData);

	instanceData->volume.Update(instanceData->transformMatrix);
}

void NzInstancedRenderable::UpdateData(InstanceData* instanceData) const
{
	NazaraAssert(instanceData, "Invalid instance data");
}

NzInstancedRenderableLibrary::LibraryMap NzInstancedRenderable::s_library;
