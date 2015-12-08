// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	AbstractRenderQueue::~AbstractRenderQueue() = default;

	void AbstractRenderQueue::AddDirectionalLight(const DirectionalLight& light)
	{
		directionalLights.push_back(light);
	}

	void AbstractRenderQueue::AddPointLight(const PointLight& light)
	{
		pointLights.push_back(light);
	}

	void AbstractRenderQueue::AddSpotLight(const SpotLight& light)
	{
		spotLights.push_back(light);
	}

	void AbstractRenderQueue::Clear(bool fully)
	{
		NazaraUnused(fully);

		directionalLights.clear();
		pointLights.clear();
		spotLights.clear();
	}
}
