// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	Light::Light(UInt8 lightType) :
	m_boundingVolume(BoundingVolumef::Null()),
	m_shadowMapFormat(Graphics::Instance()->GetPreferredDepthFormat()),
	m_shadowMapSize(512),
	m_lightType(lightType),
	m_isShadowCaster(false)
	{
	}

	Light::~Light() = default;
}
