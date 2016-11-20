// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::InstancedRenderable
	* \brief Graphics class that represents an instancer renderable
	*
	* \remark This class is abstract
	*/

	/*!
	* \brief Destructs the object and calls OnInstancedRenderableRelease
	*
	* \see OnInstancedRenderableRelease
	*/

	InstancedRenderable::~InstancedRenderable()
	{
		OnInstancedRenderableRelease(this);
	}

	/*!
	* \brief Culls the instanced if not in the frustum
	* \return true If instanced is in the frustum
	*
	* \param frustum Symbolizing the field of view
	* \param transformMatrix Matrix transformation for our object
	*/

	bool InstancedRenderable::Cull(const Frustumf& frustum, const InstanceData& instanceData) const
	{
		return frustum.Contains(instanceData.volume);
	}

	/*!
	* \brief Gets the bounding volume
	* \return Bounding volume of the instanced
	*/

	const BoundingVolumef& InstancedRenderable::GetBoundingVolume() const
	{
		EnsureBoundingVolumeUpdated();

		return m_boundingVolume;
	}

	/*!
	* \brief Invalidates data for instanced
	*
	* \param instanceData Pointer to data of instances
	* \param flags Flags for the instances
	*
	* \remark Produces a NazaraAssert if instanceData is invalid
	*/

	void InstancedRenderable::InvalidateData(InstanceData* instanceData, UInt32 flags) const
	{
		NazaraAssert(instanceData, "Invalid instance data");

		instanceData->flags |= flags;
	}

	/*!
	* \brief Updates the bounding volume
	*
	* \param instanceData Pointer to data of instances
	*
	* \remark Produces a NazaraAssert if instanceData is invalid
	*/

	void InstancedRenderable::UpdateBoundingVolume(InstanceData* instanceData) const
	{
		NazaraAssert(instanceData, "Invalid instance data");
		NazaraUnused(instanceData);

		instanceData->volume.Update(instanceData->transformMatrix);
	}

	/*!
	* \brief Updates the instance data
	*
	* \param instanceData Pointer to data of instances
	*
	* \remark Produces a NazaraAssert if instanceData is invalid
	*/

	void InstancedRenderable::UpdateData(InstanceData* instanceData) const
	{
		NazaraAssert(instanceData, "Invalid instance data");
		NazaraUnused(instanceData);
	}

	InstancedRenderableLibrary::LibraryMap InstancedRenderable::s_library;
}
