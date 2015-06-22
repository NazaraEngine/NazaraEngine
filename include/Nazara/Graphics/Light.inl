// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Renderer/Debug.hpp>

inline NzLight::NzLight(const NzLight& light) :
NzRenderable(light),
m_type(light.m_type),
m_color(light.m_color),
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

inline void NzLight::EnableShadowCasting(bool castShadows)
{
	if (m_shadowCastingEnabled != castShadows)
	{
		m_shadowCastingEnabled = castShadows;
		m_shadowMapUpdated = false;
	}
}

inline void NzLight::EnsureShadowMapUpdate() const
{
	if (!m_shadowMapUpdated)
		UpdateShadowMap();
}

inline float NzLight::GetAmbientFactor() const
{
	return m_ambientFactor;
}

inline float NzLight::GetAttenuation() const
{
	return m_attenuation;
}

inline NzColor NzLight::GetColor() const
{
	return m_color;
}

inline float NzLight::GetDiffuseFactor() const
{
	return m_diffuseFactor;
}

inline float NzLight::GetInnerAngle() const
{
	return m_innerAngle;
}

inline nzLightType NzLight::GetLightType() const
{
	return m_type;
}

inline float NzLight::GetOuterAngle() const
{
	return m_outerAngle;
}

inline float NzLight::GetOuterAngleCosine() const
{
	return m_outerAngleCosine;
}

inline float NzLight::GetOuterAngleTangent() const
{
	return m_outerAngleTangent;
}

inline float NzLight::GetRadius() const
{
	return m_radius;
}

inline NzTextureRef NzLight::GetShadowMap() const
{
	EnsureShadowMapUpdate();

	return m_shadowMap;
}

inline bool NzLight::IsShadowCastingEnabled() const
{
	return m_shadowCastingEnabled;
}

inline void NzLight::SetAmbientFactor(float factor)
{
	m_ambientFactor = factor;
}

inline void NzLight::SetAttenuation(float attenuation)
{
	m_attenuation = attenuation;
}

inline void NzLight::SetColor(const NzColor& color)
{
	m_color = color;
}

inline void NzLight::SetDiffuseFactor(float factor)
{
	m_diffuseFactor = factor;
}

inline void NzLight::SetInnerAngle(float innerAngle)
{
	m_innerAngle = innerAngle;
	m_innerAngleCosine = std::cos(NzDegreeToRadian(m_innerAngle));
}

inline void NzLight::SetLightType(nzLightType type)
{
	m_type = type;
}

inline void NzLight::SetOuterAngle(float outerAngle)
{
	m_outerAngle = outerAngle;
	m_outerAngleCosine = std::cos(NzDegreeToRadian(m_outerAngle));
	m_outerAngleTangent = std::tan(NzDegreeToRadian(m_outerAngle));

	InvalidateBoundingVolume();
}

inline void NzLight::SetRadius(float radius)
{
	m_radius = radius;

	m_invRadius = 1.f / m_radius;

	InvalidateBoundingVolume();
}

inline NzLight& NzLight::operator=(const NzLight& light)
{
	NzRenderable::operator=(light);

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
	m_shadowMapUpdated = false;
	m_type = light.m_type;

	return *this;
}

#include <Nazara/Renderer/DebugOff.hpp>
