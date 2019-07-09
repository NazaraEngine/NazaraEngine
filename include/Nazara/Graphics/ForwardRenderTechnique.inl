// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardRenderTechnique.hpp>

namespace Nz
{
	/*!
	* \brief Computes the score for directional light
	* \return 0.f
	*
	* \param object Sphere symbolising the object
	* \param light Light to compute
	*/

	inline float ForwardRenderTechnique::ComputeDirectionalLightScore(const Spheref& object, const AbstractRenderQueue::DirectionalLight& light)
	{
		NazaraUnused(object);
		NazaraUnused(light);

		///TODO: Compute a score depending on the light luminosity
		return 0.f;
	}

	/*!
	* \brief Computes the score for point light
	* \return Distance to the light
	*
	* \param object Sphere symbolising the object
	* \param light Light to compute
	*/

	inline float ForwardRenderTechnique::ComputePointLightScore(const Spheref& object, const AbstractRenderQueue::PointLight& light)
	{
		///TODO: Compute a score depending on the light luminosity
		return object.GetPosition().SquaredDistance(light.position);
	}

	/*!
	* \brief Computes the score for spot light
	* \return Distance to the light
	*
	* \param object Sphere symbolising the object
	* \param light Light to compute
	*/

	inline float ForwardRenderTechnique::ComputeSpotLightScore(const Spheref& object, const AbstractRenderQueue::SpotLight& light)
	{
		///TODO: Compute a score depending on the light luminosity and spot direction
		return object.GetPosition().SquaredDistance(light.position);
	}

	/*!
	* \brief Checks whether the directional light is suitable for the computations
	* \return true if light is enoughly close
	*
	* \param object Sphere symbolising the object
	* \param light Light to compute
	*/

	inline bool ForwardRenderTechnique::IsDirectionalLightSuitable(const Spheref& object, const AbstractRenderQueue::DirectionalLight& light)
	{
		NazaraUnused(object);
		NazaraUnused(light);

		// Directional light are always suitable
		return true;
	}

	/*!
	* \brief Checks whether the point light is suitable for the computations
	* \return true if light is close enough
	*
	* \param object Sphere symbolizing the object
	* \param light Light to compute
	*/

	inline bool ForwardRenderTechnique::IsPointLightSuitable(const Spheref& object, const AbstractRenderQueue::PointLight& light)
	{
		// If the object is too far away from this point light, there is not way it could light it
		return object.Intersect(Spheref(light.position, light.radius));
	}

	/*!
	* \brief Checks whether the spot light is suitable for the computations
	* \return true if light is close enough
	*
	* \param object Sphere symbolizing the object
	* \param light Light to compute
	*/

	inline bool ForwardRenderTechnique::IsSpotLightSuitable(const Spheref& object, const AbstractRenderQueue::SpotLight& light)
	{
		///TODO: Exclude spot lights based on their direction and outer angle?
		return object.Intersect(Spheref(light.position, light.radius));
	}
}
