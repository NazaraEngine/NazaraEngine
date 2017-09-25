// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a Light object by default
	*/

	inline Light::Light(const Light& light) :
	Renderable(light),
	m_color(light.m_color),
	m_type(light.m_type),
	m_shadowMapFormat(light.m_shadowMapFormat),
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

	/*!
	* \brief Enables shadow casting
	*
	* \param castShadows Should shadows be cast
	*/

	inline void Light::EnableShadowCasting(bool castShadows)
	{
		if (m_shadowCastingEnabled != castShadows)
		{
			m_shadowCastingEnabled = castShadows;
			m_shadowMapUpdated = false;
		}
	}

	/*!
	* \brief Ensures that the shadow map is up to date
	*/

	inline void Light::EnsureShadowMapUpdate() const
	{
		if (!m_shadowMapUpdated)
			UpdateShadowMap();
	}

	/*!
	* \brief Gets the ambient factor
	* \return Current ambient factor
	*/

	inline float Light::GetAmbientFactor() const
	{
		return m_ambientFactor;
	}

	/*!
	* \brief Gets the light attenuation (in 1 / R^2)
	* \return Attenuation
	*/

	inline float Light::GetAttenuation() const
	{
		return m_attenuation;
	}

	/*!
	* \brief Gets the color of the light
	* \return Light color
	*/

	inline Color Light::GetColor() const
	{
		return m_color;
	}

	/*!
	* \brief Gets the diffuse factor
	* \return Current diffuse factor
	*/

	inline float Light::GetDiffuseFactor() const
	{
		return m_diffuseFactor;
	}

	/*!
	* \brief Gets the inner angle in spot light
	* \return Inner angle
	*/

	inline float Light::GetInnerAngle() const
	{
		return m_innerAngle;
	}

	/*!
	* \brief Gets the cosine inner angle in spot light
	* \return Cosine inner angle
	*/

	inline float Light::GetInnerAngleCosine() const
	{
		return m_innerAngleCosine;
	}

	/*!
	* \brief Gets the inverse of the radius
	* \return Inverse of the radius
	*/

	inline float Light::GetInvRadius() const
	{
		return m_invRadius;
	}

	/*!
	* \brief Gets the type of the light
	* \return Light type
	*/

	inline LightType Light::GetLightType() const
	{
		return m_type;
	}

	/*!
	* \brief Gets the outer angle in spot light
	* \return Outer angle
	*/

	inline float Light::GetOuterAngle() const
	{
		return m_outerAngle;
	}

	/*!
	* \brief Gets the cosine outer angle in spot light
	* \return Cosine outer angle
	*/

	inline float Light::GetOuterAngleCosine() const
	{
		return m_outerAngleCosine;
	}

	/*!
	* \brief Gets the tangent outer angle in spot light
	* \return Tangent outer angle
	*/

	inline float Light::GetOuterAngleTangent() const
	{
		return m_outerAngleTangent;
	}

	/*!
	* \brief Gets the radius of the light
	* \return Light radius
	*/

	inline float Light::GetRadius() const
	{
		return m_radius;
	}

	/*!
	* \brief Gets the shadow map
	* \return Reference to the shadow map texture
	*/

	inline TextureRef Light::GetShadowMap() const
	{
		EnsureShadowMapUpdate();

		return m_shadowMap;
	}

	/*!
	* \brief Gets the format of the shadow map
	* \return Shadow map format
	*/

	inline PixelFormatType Light::GetShadowMapFormat() const
	{
		return m_shadowMapFormat;
	}

	/*!
	* \brief Gets the size of the shadow map
	* \return Shadow map size
	*/

	inline const Vector2ui& Light::GetShadowMapSize() const
	{
		return m_shadowMapSize;
	}

	/*!
	* \brief Checks whether the shadow casting is enabled
	* \return true If it is the case
	*/

	inline bool Light::IsShadowCastingEnabled() const
	{
		return m_shadowCastingEnabled;
	}

	/*!
	* \brief Sets the ambient factor
	*
	* \param factor Ambient factor
	*/

	inline void Light::SetAmbientFactor(float factor)
	{
		m_ambientFactor = factor;
	}

	/*!
	* \brief Sets the light attenuation (in 1 / R^2)
	*
	* \param attenuation Light attenuation
	*/

	inline void Light::SetAttenuation(float attenuation)
	{
		m_attenuation = attenuation;
	}

	/*!
	* \brief Sets the color of the light
	*
	* \param color Light color
	*/

	inline void Light::SetColor(const Color& color)
	{
		m_color = color;
	}

	/*!
	* \brief Sets the diffuse factor
	*
	* \param factor Diffuse factor
	*/

	inline void Light::SetDiffuseFactor(float factor)
	{
		m_diffuseFactor = factor;
	}

	/*!
	* \brief Sets the inner angle in spot light
	* \return innerAngle Inner angle
	*/

	inline void Light::SetInnerAngle(float innerAngle)
	{
		m_innerAngle = innerAngle;
		m_innerAngleCosine = std::cos(DegreeToRadian(m_innerAngle));
	}

	/*!
	* \brief Sets the type of light
	*
	* \param type Light type
	*/

	inline void Light::SetLightType(LightType type)
	{
		m_type = type;

		InvalidateShadowMap();
	}

	/*!
	* \brief Sets the outer angle in spot light
	* \return outerAngle Outer angle
	*
	* \remark Invalidates the bounding volume
	*/

	inline void Light::SetOuterAngle(float outerAngle)
	{
		m_outerAngle = outerAngle;
		m_outerAngleCosine = std::cos(DegreeToRadian(m_outerAngle));
		m_outerAngleTangent = std::tan(DegreeToRadian(m_outerAngle));

		InvalidateBoundingVolume();
	}

	/*!
	* \brief Sets the radius of the light
	* \return radius Light radius
	*
	* \remark Invalidates the bounding volume
	*/

	inline void Light::SetRadius(float radius)
	{
		m_radius = radius;

		m_invRadius = 1.f / m_radius;

		InvalidateBoundingVolume();
	}

	/*!
	* \brief Sets the shadow map format
	*
	* \param shadowFormat Shadow map format
	*
	* \remark Invalidates the shadow map
	* \remark Produces a NazaraAssert if format is not a depth type
	*/

	inline void Light::SetShadowMapFormat(PixelFormatType shadowFormat)
	{
		NazaraAssert(PixelFormat::GetContent(shadowFormat) == PixelFormatContent_DepthStencil, "Shadow format type is not a depth format");

		m_shadowMapFormat = shadowFormat;

		InvalidateShadowMap();
	}

	/*!
	* \brief Sets the size of the shadow map
	*
	* \param size Shadow map size
	*
	* \remark Invalidates the shadow map
	* \remark Produces a NazaraAssert if size is zero
	*/

	inline void Light::SetShadowMapSize(const Vector2ui& size)
	{
		NazaraAssert(size.x > 0 && size.y > 0, "Shadow map size must have a positive size");

		m_shadowMapSize = size;

		InvalidateShadowMap();
	}

	/*!
	* \brief Sets the current light with the content of the other one
	* \return A reference to this
	*
	* \param light The other Light
	*
	* \remark Invalidates the shadow map
	*/

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

	/*!
	* \brief Invalidates the shadow map
	*/

	inline void Light::InvalidateShadowMap()
	{
		m_shadowMapUpdated = false;
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
