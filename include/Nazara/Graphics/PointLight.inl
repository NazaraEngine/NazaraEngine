// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/Enums.hpp>
#include <cassert>

namespace Nz
{
	inline PointLight::PointLight() :
	Light(SafeCast<UInt8>(BasicLightType::Point)),
	m_color(Color::White()),
	m_position(Vector3f::Zero()),
	m_ambientFactor(0.2f),
	m_diffuseFactor(1.f)
	{
		UpdateRadius(5.f);
	}

	inline float PointLight::GetAmbientFactor() const
	{
		return m_ambientFactor;
	}

	inline Color PointLight::GetColor() const
	{
		return m_color;
	}

	inline float PointLight::GetDiffuseFactor() const
	{
		return m_diffuseFactor;
	}

	inline const Vector3f& PointLight::GetPosition() const
	{
		return m_position;
	}

	inline float PointLight::GetInvRadius() const
	{
		return m_invRadius;
	}

	inline float PointLight::GetRadius() const
	{
		return m_radius;
	}

	inline void PointLight::UpdateAmbientFactor(float factor)
	{
		m_ambientFactor = factor;

		OnLightDataInvalided(this);
	}

	inline void PointLight::UpdateColor(Color color)
	{
		m_color = color;

		OnLightDataInvalided(this);
	}

	inline void PointLight::UpdateDiffuseFactor(float factor)
	{
		m_diffuseFactor = factor;

		OnLightDataInvalided(this);
	}

	inline void PointLight::UpdatePosition(const Vector3f& position)
	{
		m_position = position;

		UpdateBoundingVolume();
		OnLightTransformInvalided(this);
	}

	inline void PointLight::UpdateRadius(float radius)
	{
		m_radius = radius;
		m_invRadius = 1.f / m_radius;

		UpdateBoundingVolume();
	}

	inline void PointLight::UpdateBoundingVolume()
	{
		Vector3f extent = Vector3f(m_radius, m_radius, m_radius) * Sqrt3<float>();
		BoundingVolumef boundingVolume(Boxf(-extent * 0.5f, extent));
		boundingVolume.Update(m_position);

		Light::UpdateBoundingVolume(boundingVolume); //< will trigger OnLightDataInvalided
	}
}
