// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
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
		m_innerAngleCosine = std::cos(NzDegreeToRadian(m_innerAngle));
	}

	inline void Light::SetLightType(LightType type)
	{
		m_type = type;
	}

	inline void Light::SetOuterAngle(float outerAngle)
	{
		m_outerAngle = outerAngle;
		m_outerAngleCosine = std::cos(NzDegreeToRadian(m_outerAngle));
		m_outerAngleTangent = std::tan(NzDegreeToRadian(m_outerAngle));

		InvalidateBoundingVolume();
	}

	inline void Light::SetRadius(float radius)
	{
		m_radius = radius;

		m_invRadius = 1.f / m_radius;

		InvalidateBoundingVolume();
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
