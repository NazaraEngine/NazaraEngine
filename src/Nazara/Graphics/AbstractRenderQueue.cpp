// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::AbstractRenderQueue
	* \brief Graphics class that represents the rendering queue for our scene
	*
	* \remark This class is abstract
	*/

	AbstractRenderQueue::~AbstractRenderQueue() = default;

	/*!
	* \brief Adds a directional light to the rendering queue
	*
	* \param light Directional light
	*/

	void AbstractRenderQueue::AddDirectionalLight(const DirectionalLight& light)
	{
		directionalLights.push_back(light);
	}

	/*!
	* \brief Adds a point light to the rendering queue
	*
	* \param light Point light
	*/

	void AbstractRenderQueue::AddPointLight(const PointLight& light)
	{
		pointLights.push_back(light);
	}

	/*!
	* \brief Adds a spot light to the rendering queue
	*
	* \param light Spot light
	*/

	void AbstractRenderQueue::AddSpotLight(const SpotLight& light)
	{
		spotLights.push_back(light);
	}

	/*!
	* \brief Clears the rendering queue
	*
	* \param fully Should everything be cleared ?
	*/

	void AbstractRenderQueue::Clear(bool fully)
	{
		NazaraUnused(fully);

		directionalLights.clear();
		pointLights.clear();
		spotLights.clear();
	}
}
