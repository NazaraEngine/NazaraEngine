// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline Light::Light(const Light& light) :
	Renderable(light),
	m_type(light.m_type),
	m_shadowMapFormat(light.m_shadowMapFormat),
	m_color(light.m_color),
	m_shadowMapSize(light.m_shadowMapSize),
	m_shadowCastingEnabled(light.m_shadowCastingEnabled),
	m_shadowMapUpdated(false),
	m_ambientFactor(light.m_ambientFactor),
	m_attenuation(light.m_attenuation),
	m_diffuseFactor(light.m_diffuseFactor),
	m_innerAngle(light.m_innerAngle),
	m_innerAngleCosine(light.m_innerAngleCosine),
	m_invRadius(light.m_invRadius),
	m_outerAngle(light.m_outerAngle),
	m_outerAngleCosine(light.m_outerAngleCosine),
	m_outerAngleTangent(light.m_outerAngleTangent),
	m_radius(light.m_radius)
	{
	}

	inline void Light::EnableShadowCasting(bool castShadows)
	{
		if (m_shadowCastingEnabled != castShadows)
		{
			m_shadowCastingEnabled = castShadows;
			m_shadowMapUpdated = false;
		}
	}

	inline void Light::EnsureShadowMapUpdate() const
	{
		if (!m_shadowMapUpdated)
			UpdateShadowMap();
	}

	inline float Light::GetAmbientFactor() const
	{
		return m_ambientFactor;
	}

	inline float Light::GetAttenuation() const
	{
		return m_attenuation;
	}

	inline Color Light::GetColor() const
	{
		return m_color;
	}

	inline float Light::GetDiffuseFactor() const
	{
		return m_diffuseFactor;
	}

	inline float Light::GetInnerAngle() const
	{
		return m_innerAngle;
	}

	inline float Light::GetInnerAngleCosine() const
	{
		return m_innerAngleCosine;
	}

	inline float Light::GetInvRadius() const
	{
		return m_invRadius;
	}

	inline LightType Light::GetLightType() const
	{
		return m_type;
	}

	inline float Light::GetOuterAngle() const
	{
		return m_outerAngle;
	}

	inline float Light::GetOuterAngleCosine() const
	{
		return m_outerAngleCosine;
	}

	inline float Light::GetOuterAngleTangent() const
	{
		return m_outerAngleTangent;
	}

	inline float Light::GetRadius() const
	{
		return m_radius;
	}

	inline TextureRef Light::GetShadowMap() const
	{
		EnsureShadowMapUpdate();

		return m_shadowMap;
	}

	inline PixelFormatType Light::GetShadowMapFormat() const
	{
		return m_shadowMapFormat;
	}

	inline const Vector2ui& Light::GetShadowMapSize() const
	{
		return m_shadowMapSize;
	}

	inline bool Light::IsShadowCastingEnabled() const
	{
		return m_shadowCastingEnabled;
	}

	inline void Light::SetAmbientFactor(float factor)
	{
		m_ambientFactor = factor;
	}

	inline void Light::SetAttenuation(float attenuation)
	{
		m_attenuation = attenuation;
	}

	inline void Light::SetColor(const Color& color)
	{
		m_color = color;
	}

	inline void Light::SetDiffuseFactor(float factor)
	{
		m_diffuseFactor = factor;
	}

	inline void Light::SetInnerAngle(float innerAngle)
	{
		m_innerAngle = innerAngle;
		m_innerAngleCosine = std::cos(DegreeToRadian(m_innerAngle));
	}

	inline void Light::SetLightType(LightType type)
	{
		m_type = type;

		InvalidateShadowMap();
	}

	inline void Light::SetOuterAngle(float outerAngle)
	{
		m_outerAngle = outerAngle;
		m_outerAngleCosine = std::cos(DegreeToRadian(m_outerAngle));
		m_outerAngleTangent = std::tan(DegreeToRadian(m_outerAngle));

		InvalidateBoundingVolume();
	}

	inline void Light::SetRadius(float radius)
	{
		m_radius = radius;

		m_invRadius = 1.f / m_radius;

		InvalidateBoundingVolume();
	}

	inline void Light::SetShadowMapFormat(PixelFormatType shadowFormat)
	{
		NazaraAssert(PixelFormat::GetContent(shadowFormat) == PixelFormatContent_DepthStencil, "Shadow format type is not a depth format");

		m_shadowMapFormat = shadowFormat;

		InvalidateShadowMap();
	}

	inline void Light::SetShadowMapSize(const Vector2ui& size)
	{
		NazaraAssert(size.x > 0 && size.y > 0, "Shadow map size must have a positive size");

		m_shadowMapSize = size;

		InvalidateShadowMap();
	}

	inline Light& Light::operator=(const Light& light)
	{
		Renderable::operator=(light);

		m_ambientFactor = light.m_ambientFactor;
		m_attenuation = light.m_attenuation;
		m_color = light.m_color;
		m_diffuseFactor = light.m_diffuseFactor;
		m_innerAngle = light.m_innerAngle;
		m_innerAngleCosine = light.m_innerAngleCosine;
		m_invRadius = light.m_invRadius;
		m_outerAngle = light.m_outerAngle;
		m_outerAngleCosine = light.m_outerAngleCosine;
		m_outerAngleTangent = light.m_outerAngleTangent;
		m_radius = light.m_radius;
		m_shadowCastingEnabled = light.m_shadowCastingEnabled;
		m_shadowMapFormat = light.m_shadowMapFormat;
		m_shadowMapSize = light.m_shadowMapSize;
		m_type = light.m_type;

		InvalidateShadowMap();
		return *this;
	}

	inline void Light::InvalidateShadowMap()
	{
		m_shadowMapUpdated = false;
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
