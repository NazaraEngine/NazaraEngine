// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Renderable.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::Renderable
	* \brief Graphics class that represents a renderable element for our scene
	*
	* \remark This class is abstract
	*/

	Renderable::~Renderable() = default;

	/*!
	* \brief Culls the model if not in the frustum
	* \return true If renderable is in the frustum
	*
	* \param frustum Symbolizing the field of view
	* \param transformMatrix Matrix transformation for our object
	*/

	bool Renderable::Cull(const Frustumf& frustum, const Matrix4f& transformMatrix) const
	{
		NazaraUnused(transformMatrix);

		return frustum.Contains(m_boundingVolume);
	}

	/*!
	* \brief Gets the bounding volume
	* \return Bounding volume of the renderable element
	*/

	const BoundingVolumef& Renderable::GetBoundingVolume() const
	{
		EnsureBoundingVolumeUpdated();

		return m_boundingVolume;
	}

	/*!
	* \brief Updates the bounding volume by a matrix
	*
	* \param transformMatrix Matrix transformation for our bounding volume
	*/

	void Renderable::UpdateBoundingVolume(const Matrix4f& transformMatrix)
	{
		m_boundingVolume.Update(transformMatrix);
	}
}
