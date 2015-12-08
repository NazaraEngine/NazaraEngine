// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Renderable.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	Renderable::~Renderable() = default;

	bool Renderable::Cull(const Frustumf& frustum, const Matrix4f& transformMatrix) const
	{
		NazaraUnused(transformMatrix);

		return frustum.Contains(m_boundingVolume);
	}

	const BoundingVolumef& Renderable::GetBoundingVolume() const
	{
		EnsureBoundingVolumeUpdated();

		return m_boundingVolume;
	}

	void Renderable::UpdateBoundingVolume(const Matrix4f& transformMatrix)
	{
		m_boundingVolume.Update(transformMatrix);
	}
}
