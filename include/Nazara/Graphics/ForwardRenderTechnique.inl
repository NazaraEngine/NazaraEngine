// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Renderer.hpp>

namespace Nz
{
	/*!
	* \brief Sens the uniforms for light
	*
	* \param shader Shader to send uniforms to
	* \param uniforms Uniforms to send
	* \param index Index of the light
	* \param uniformOffset Offset for the uniform
	* \param availableTextureUnit Unit texture available
	*/

	inline void ForwardRenderTechnique::SendLightUniforms(const Shader* shader, const LightUniforms& uniforms, unsigned int index, unsigned int uniformOffset, UInt8 availableTextureUnit) const
	{
		// If anyone got a better idea..
		int dummyCubemap = Renderer::GetMaxTextureUnits() - 1;
		int dummyTexture = Renderer::GetMaxTextureUnits() - 2;

		if (index < m_lights.size())
		{
			const LightIndex& lightIndex = m_lights[index];

			shader->SendInteger(uniforms.locations.type + uniformOffset, lightIndex.type); //< Sends the light type

			switch (lightIndex.type)
			{
				case LightType_Directional:
				{
					const auto& light = m_renderQueue.directionalLights[lightIndex.index];

					shader->SendColor(uniforms.locations.color + uniformOffset, light.color);
					shader->SendVector(uniforms.locations.factors + uniformOffset, Vector2f(light.ambientFactor, light.diffuseFactor));
					shader->SendVector(uniforms.locations.parameters1 + uniformOffset, Vector4f(light.direction));

					shader->SendBoolean(uniforms.locations.shadowMapping + uniformOffset, light.shadowMap != nullptr);
					if (light.shadowMap)
					{
						Renderer::SetTexture(availableTextureUnit, light.shadowMap);
						Renderer::SetTextureSampler(availableTextureUnit, s_shadowSampler);

						shader->SendMatrix(uniforms.locations.lightViewProjMatrix + index, light.transformMatrix);
						shader->SendInteger(uniforms.locations.directionalSpotLightShadowMap + index, availableTextureUnit);
					}
					else
						shader->SendInteger(uniforms.locations.directionalSpotLightShadowMap + index, dummyTexture);

					shader->SendInteger(uniforms.locations.pointLightShadowMap + index, dummyCubemap);
					break;
				}

				case LightType_Point:
				{
					const auto& light = m_renderQueue.pointLights[lightIndex.index];

					shader->SendColor(uniforms.locations.color + uniformOffset, light.color);
					shader->SendVector(uniforms.locations.factors + uniformOffset, Vector2f(light.ambientFactor, light.diffuseFactor));
					shader->SendVector(uniforms.locations.parameters1 + uniformOffset, Vector4f(light.position, light.attenuation));
					shader->SendVector(uniforms.locations.parameters2 + uniformOffset, Vector4f(0.f, 0.f, 0.f, light.invRadius));

					shader->SendBoolean(uniforms.locations.shadowMapping + uniformOffset, light.shadowMap != nullptr);
					if (light.shadowMap)
					{
						Renderer::SetTexture(availableTextureUnit, light.shadowMap);
						Renderer::SetTextureSampler(availableTextureUnit, s_shadowSampler);

						shader->SendInteger(uniforms.locations.pointLightShadowMap + index, availableTextureUnit);
					}
					else
						shader->SendInteger(uniforms.locations.pointLightShadowMap + index, dummyCubemap);

					shader->SendInteger(uniforms.locations.directionalSpotLightShadowMap + index, dummyTexture);
					break;
				}

				case LightType_Spot:
				{
					const auto& light = m_renderQueue.spotLights[lightIndex.index];

					shader->SendColor(uniforms.locations.color + uniformOffset, light.color);
					shader->SendVector(uniforms.locations.factors + uniformOffset, Vector2f(light.ambientFactor, light.diffuseFactor));
					shader->SendVector(uniforms.locations.parameters1 + uniformOffset, Vector4f(light.position, light.attenuation));
					shader->SendVector(uniforms.locations.parameters2 + uniformOffset, Vector4f(light.direction, light.invRadius));
					shader->SendVector(uniforms.locations.parameters3 + uniformOffset, Vector2f(light.innerAngleCosine, light.outerAngleCosine));

					shader->SendBoolean(uniforms.locations.shadowMapping + uniformOffset, light.shadowMap != nullptr);
					if (light.shadowMap)
					{
						Renderer::SetTexture(availableTextureUnit, light.shadowMap);
						Renderer::SetTextureSampler(availableTextureUnit, s_shadowSampler);

						shader->SendMatrix(uniforms.locations.lightViewProjMatrix + index, light.transformMatrix);
						shader->SendInteger(uniforms.locations.directionalSpotLightShadowMap + index, availableTextureUnit);
					}
					else
						shader->SendInteger(uniforms.locations.directionalSpotLightShadowMap + index, dummyTexture);

					shader->SendInteger(uniforms.locations.pointLightShadowMap + index, dummyCubemap);

					break;
				}
			}
		}
		else
		{
			shader->SendInteger(uniforms.locations.type + uniformOffset, -1); //< Disable the light in the shader
			shader->SendInteger(uniforms.locations.directionalSpotLightShadowMap + index, dummyTexture);
			shader->SendInteger(uniforms.locations.pointLightShadowMap + index, dummyCubemap);
		}
	}

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
		return object.SquaredDistance(light.position);
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
		return object.SquaredDistance(light.position);
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
	* \return true if light is enoughly close
	*
	* \param object Sphere symbolising the object
	* \param light Light to compute
	*/

	inline bool ForwardRenderTechnique::IsPointLightSuitable(const Spheref& object, const AbstractRenderQueue::PointLight& light)
	{
		// If the object is too far away from this point light, there is not way it could light it
		return object.SquaredDistance(light.position) <= light.radius * light.radius;
	}

	/*!
	* \brief Checks whether the spot light is suitable for the computations
	* \return true if light is enoughly close
	*
	* \param object Sphere symbolising the object
	* \param light Light to compute
	*/

	inline bool ForwardRenderTechnique::IsSpotLightSuitable(const Spheref& object, const AbstractRenderQueue::SpotLight& light)
	{
		///TODO: Exclude spot lights based on their direction and outer angle?
		return object.SquaredDistance(light.position) <= light.radius * light.radius;
	}
}
