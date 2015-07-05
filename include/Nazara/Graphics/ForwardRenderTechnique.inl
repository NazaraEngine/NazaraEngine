// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Renderer.hpp>

inline void NzForwardRenderTechnique::SendLightUniforms(const NzShader* shader, const NzLightUniforms& uniforms, unsigned int index, unsigned int uniformOffset, nzUInt8 availableTextureUnit) const
{
	// If anyone got a better idea..
	int dummyCubemap = NzRenderer::GetMaxTextureUnits() - 1;
	int dummyTexture = NzRenderer::GetMaxTextureUnits() - 2;

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

				shader->SendBoolean(uniforms.locations.shadowMapping + uniformOffset, light.shadowMap != nullptr);
				if (light.shadowMap)
				{
					NzRenderer::SetTexture(availableTextureUnit, light.shadowMap);
					NzRenderer::SetTextureSampler(availableTextureUnit, s_shadowSampler);

					shader->SendInteger(uniforms.locations.pointLightShadowMap + index, availableTextureUnit);
				}
				else
					shader->SendInteger(uniforms.locations.pointLightShadowMap + index, dummyCubemap);

				shader->SendInteger(uniforms.locations.spotLightShadowMap + index, dummyTexture);
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

				shader->SendBoolean(uniforms.locations.shadowMapping + uniformOffset, light.shadowMap != nullptr);
				if (light.shadowMap)
				{
					NzRenderer::SetTexture(availableTextureUnit, light.shadowMap);
					NzRenderer::SetTextureSampler(availableTextureUnit, s_shadowSampler);

					shader->SendMatrix(uniforms.locations.lightViewProjMatrix + index, light.transformMatrix);
					shader->SendInteger(uniforms.locations.spotLightShadowMap + index, availableTextureUnit);
				}
				else
					shader->SendInteger(uniforms.locations.spotLightShadowMap + index, dummyTexture);

				shader->SendInteger(uniforms.locations.pointLightShadowMap + index, dummyCubemap);
				break;
			}
		}
	}
	else
	{
		shader->SendInteger(uniforms.locations.type + uniformOffset, -1); //< Disable the light in the shader
		shader->SendInteger(uniforms.locations.pointLightShadowMap + index, dummyCubemap);
		shader->SendInteger(uniforms.locations.spotLightShadowMap + index, dummyTexture);
	}
}

inline float NzForwardRenderTechnique::ComputeDirectionalLightScore(const NzSpheref& object, const NzAbstractRenderQueue::DirectionalLight& light)
{
	NazaraUnused(object);
	NazaraUnused(light);

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
	NazaraUnused(object);
	NazaraUnused(light);

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
