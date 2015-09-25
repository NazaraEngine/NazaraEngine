// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

namespace Nz
{
	inline void Renderable::EnsureBoundingVolumeUpdated() const
	{
		if (!m_boundingVolumeUpdated)
			UpdateBoundingVolume();
	}

	inline void Renderable::InvalidateBoundingVolume()
	{
		m_boundingVolumeUpdated = false;
	}

	inline void Renderable::UpdateBoundingVolume() const
	{
		MakeBoundingVolume();
		m_boundingVolumeUpdated = true;
	}
}
