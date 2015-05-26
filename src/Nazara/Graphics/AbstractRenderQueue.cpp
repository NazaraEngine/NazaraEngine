// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzAbstractRenderQueue::~NzAbstractRenderQueue() = default;

void NzAbstractRenderQueue::AddDirectionalLight(const NzColor& color, float ambientFactor, float diffuseFactor, const NzVector3f& direction)
{
	directionalLights.push_back(DirectionalLight{color, direction, ambientFactor, diffuseFactor});
}

void NzAbstractRenderQueue::AddPointLight(const NzColor& color, float ambientFactor, float diffuseFactor, const NzVector3f& position, float radius, float attenuation)
{
	pointLights.push_back(PointLight{color, position, ambientFactor, attenuation, diffuseFactor, radius});
}

void NzAbstractRenderQueue::AddSpotLight(const NzColor& color, float ambientFactor, float diffuseFactor, const NzVector3f& position, const NzVector3f& direction, float radius, float attenuation, float innerAngle, float outerAngle)
{
	spotLights.push_back(SpotLight{color, direction, position, ambientFactor, attenuation, diffuseFactor, innerAngle, outerAngle, radius});
}

void NzAbstractRenderQueue::Clear(bool fully)
{
	directionalLights.clear();
	pointLights.clear();
	spotLights.clear();
}
