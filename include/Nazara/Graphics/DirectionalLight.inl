// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Enums.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline DirectionalLight::DirectionalLight() :
	Light(SafeCast<UInt8>(BasicLightType::Directional)),
	m_color(Color::White()),
	m_ambientFactor(0.2f),
	m_diffuseFactor(1.f)
	{
		UpdateRotation(Quaternionf::Identity());
	}

	inline float DirectionalLight::GetAmbientFactor() const
	{
		return m_ambientFactor;
	}

	inline Color DirectionalLight::GetColor() const
	{
		return m_color;
	}

	inline const Vector3f& DirectionalLight::GetDirection() const
	{
		return m_direction;
	}

	inline const Quaternionf& DirectionalLight::GetRotation() const
	{
		return m_rotation;
	}

	inline float DirectionalLight::GetDiffuseFactor() const
	{
		return m_diffuseFactor;
	}

	inline void DirectionalLight::UpdateAmbientFactor(float factor)
	{
		m_ambientFactor = factor;

		OnLightDataInvalided(this);
	}

	inline void DirectionalLight::UpdateColor(Color color)
	{
		m_color = color;

		OnLightDataInvalided(this);
	}

	inline void DirectionalLight::UpdateDiffuseFactor(float factor)
	{
		m_diffuseFactor = factor;

		OnLightDataInvalided(this);
	}

	inline void DirectionalLight::UpdateDirection(const Vector3f& direction)
	{
		UpdateRotation(Quaternionf::RotationBetween(Vector3f::Forward(), direction));
	}

	inline void DirectionalLight::UpdateRotation(const Quaternionf& rotation)
	{
		m_rotation = rotation;
		m_direction = rotation * Vector3f::Forward();

		UpdateBoundingVolume();
	}

	inline void DirectionalLight::UpdateBoundingVolume()
	{
		Light::UpdateBoundingVolume(BoundingVolumef::Infinite()); //< will trigger OnLightDataInvalided
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
