// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

inline void NzForwardRenderTechnique::SendLightUniforms(const NzShader* shader, const NzLightUniforms& uniforms, unsigned int uniformOffset, unsigned int index) const
{
	if (index < m_lights.size())
	{
		const LightIndex& lightIndex = m_lights[index];

		shader->SendInteger(uniforms.locations.type + uniformOffset, lightIndex.type); //< Sends the light type

		switch (lightIndex.type)
		{
			case nzLightType_Directional:
			{
				const auto& light = m_renderQueue.directionalLights[lightIndex.index];

				shader->SendColor(uniforms.locations.color + uniformOffset, light.color);
				shader->SendVector(uniforms.locations.factors + uniformOffset, NzVector2f(light.ambientFactor, light.diffuseFactor));
				shader->SendVector(uniforms.locations.parameters1 + uniformOffset, NzVector4f(light.direction));
				break;
			}

			case nzLightType_Point:
			{
				const auto& light = m_renderQueue.pointLights[lightIndex.index];

				shader->SendColor(uniforms.locations.color + uniformOffset, light.color);
				shader->SendVector(uniforms.locations.factors + uniformOffset, NzVector2f(light.ambientFactor, light.diffuseFactor));
				shader->SendVector(uniforms.locations.parameters1 + uniformOffset, NzVector4f(light.position, light.attenuation));
				shader->SendVector(uniforms.locations.parameters2 + uniformOffset, NzVector4f(0.f, 0.f, 0.f, light.invRadius));
				break;
			}

			case nzLightType_Spot:
			{
				const auto& light = m_renderQueue.spotLights[lightIndex.index];

				shader->SendColor(uniforms.locations.color + uniformOffset, light.color);
				shader->SendVector(uniforms.locations.factors + uniformOffset, NzVector2f(light.ambientFactor, light.diffuseFactor));
				shader->SendVector(uniforms.locations.parameters1 + uniformOffset, NzVector4f(light.position, light.attenuation));
				shader->SendVector(uniforms.locations.parameters2 + uniformOffset, NzVector4f(light.direction, light.invRadius));
				shader->SendVector(uniforms.locations.parameters3 + uniformOffset, NzVector2f(light.innerAngleCosine, light.outerAngleCosine));
				break;
			}
		}
	}
	else
		shader->SendInteger(uniforms.locations.type + uniformOffset, -1); //< Disable the light in the shader
}

inline float NzForwardRenderTechnique::ComputeDirectionalLightScore(const NzSpheref& object, const NzAbstractRenderQueue::DirectionalLight& light)
{
	///TODO: Compute a score depending on the light luminosity
	return 0.f;
}

inline float NzForwardRenderTechnique::ComputePointLightScore(const NzSpheref& object, const NzAbstractRenderQueue::PointLight& light)
{
	///TODO: Compute a score depending on the light luminosity
	return object.SquaredDistance(light.position);
}

inline float NzForwardRenderTechnique::ComputeSpotLightScore(const NzSpheref& object, const NzAbstractRenderQueue::SpotLight& light)
{
	///TODO: Compute a score depending on the light luminosity and spot direction
	return object.SquaredDistance(light.position);
}

inline bool NzForwardRenderTechnique::IsDirectionalLightSuitable(const NzSpheref& object, const NzAbstractRenderQueue::DirectionalLight& light)
{
	// Directional light are always suitables
	return true;
}

inline bool NzForwardRenderTechnique::IsPointLightSuitable(const NzSpheref& object, const NzAbstractRenderQueue::PointLight& light)
{
	// If the object is too far away from this point light, there is not way it could light it
	return object.SquaredDistance(light.position) <= light.radius * light.radius;
}

inline bool NzForwardRenderTechnique::IsSpotLightSuitable(const NzSpheref& object, const NzAbstractRenderQueue::SpotLight& light)
{
	///TODO: Exclude spot lights based on their direction and outer angle?
	return object.SquaredDistance(light.position) <= light.radius * light.radius;
}
