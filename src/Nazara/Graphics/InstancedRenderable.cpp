// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	InstancedRenderable::~InstancedRenderable()
	{
		OnInstancedRenderableRelease(this);
	}

	bool InstancedRenderable::Cull(const Frustumf& frustum, const InstanceData& instanceData) const
	{
		return frustum.Contains(instanceData.volume);
	}

	const BoundingVolumef& InstancedRenderable::GetBoundingVolume() const
	{
		EnsureBoundingVolumeUpdated();

		return m_boundingVolume;
	}

	void InstancedRenderable::InvalidateData(InstanceData* instanceData, UInt32 flags) const
	{
		instanceData->flags |= flags;
	}

	void InstancedRenderable::UpdateBoundingVolume(InstanceData* instanceData) const
	{
		NazaraAssert(instanceData, "Invalid instance data");
		NazaraUnused(instanceData);

		instanceData->volume.Update(instanceData->transformMatrix);
	}

	void InstancedRenderable::UpdateData(InstanceData* instanceData) const
	{
		NazaraAssert(instanceData, "Invalid instance data");
	}

	InstancedRenderableLibrary::LibraryMap InstancedRenderable::s_library;
}
