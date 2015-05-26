// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzAbstractRenderQueue::~NzAbstractRenderQueue() = default;

void NzAbstractRenderQueue::AddDirectionalLight(const DirectionalLight& light)
{
	directionalLights.push_back(light);
}

void NzAbstractRenderQueue::AddPointLight(const PointLight& light)
{
	pointLights.push_back(light);
}

void NzAbstractRenderQueue::AddSpotLight(const SpotLight& light)
{
	spotLights.push_back(light);
}

void NzAbstractRenderQueue::Clear(bool fully)
{
	directionalLights.clear();
	pointLights.clear();
	spotLights.clear();
}
