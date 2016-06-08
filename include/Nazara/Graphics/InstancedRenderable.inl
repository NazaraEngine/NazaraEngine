// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

namespace Nz
{
	/*!
	* \brief Constructs a InstancedRenderable object by assignation
	*
	* \param renderable InstancedRenderable to copy into this
	*/

	inline InstancedRenderable::InstancedRenderable(const InstancedRenderable& renderable) :
	RefCounted(),
	m_boundingVolume(renderable.m_boundingVolume),
	m_boundingVolumeUpdated(renderable.m_boundingVolumeUpdated)
	{
	}

	/*!
	* \brief Ensures that the bounding volume is up to date
	*/

	inline void InstancedRenderable::EnsureBoundingVolumeUpdated() const
	{
		if (!m_boundingVolumeUpdated)
			UpdateBoundingVolume();
	}

	/*!
	* \brief Invalidates the bounding volume
	*/

	inline void InstancedRenderable::InvalidateBoundingVolume()
	{
		m_boundingVolumeUpdated = false;
	}

	/*!
	* \brief Invalidates the instance data based on flags
	*
	* \param flags Flags to invalidate
	*/

	inline void InstancedRenderable::InvalidateInstanceData(UInt32 flags)
	{
		OnInstancedRenderableInvalidateData(this, flags);
	}

	/*!
	* \brief Sets the current instanced renderable with the content of the other one
	* \return A reference to this
	*
	* \param renderable The other InstancedRenderable
	*/

	inline InstancedRenderable& InstancedRenderable::operator=(const InstancedRenderable& renderable)
	{
		m_boundingVolume = renderable.m_boundingVolume;
		m_boundingVolumeUpdated = renderable.m_boundingVolumeUpdated;

		return *this;
	}

	/*!
	* \brief Updates the bounding volume
	*/

	inline void InstancedRenderable::UpdateBoundingVolume() const
	{
		MakeBoundingVolume();
		m_boundingVolumeUpdated = true;
	}
}
