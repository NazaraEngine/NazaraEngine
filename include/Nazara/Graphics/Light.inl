// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Light.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline Light::Light() :
	m_boundingVolume(BoundingVolumef::Null())
	{
	}

	inline const BoundingVolumef& Light::GetBoundingVolume() const
	{
		return m_boundingVolume;
	}

	inline void Light::UpdateBoundingVolume(const BoundingVolumef& boundingVolume)
	{
		m_boundingVolume = boundingVolume;

		OnLightDataInvalided(this);
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
