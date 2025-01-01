// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/Enums.hpp>

namespace Nz
{
	inline DirectionalLight::DirectionalLight() :
	Light(SafeCast<UInt8>(BasicLightType::Directional)),
	m_color(Color::White()),
	m_ambientFactor(0.2f),
	m_diffuseFactor(1.f),
	m_shadowCascadeSplitLambda(0.97f),
	m_shadowCascadeFixedSplit(false)
	{
		m_fixedSplitFactors.push_back(0.05f);
		m_fixedSplitFactors.push_back(0.2f);
		m_fixedSplitFactors.push_back(0.5f);
		m_fixedSplitFactors.push_back(0.8f);

		UpdateRotation(Quaternionf::Identity());
	}

	inline void DirectionalLight::EnableFixedShadowCascadSplit(bool enable)
	{
		m_shadowCascadeFixedSplit = enable;
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

	inline float DirectionalLight::GetShadowCascadeSplitLambda() const
	{
		return m_shadowCascadeSplitLambda;
	}

	inline std::span<const float> DirectionalLight::GetShadowCascadeFixedSplitFactors() const
	{
		return std::span<const float>(m_fixedSplitFactors.data(), m_fixedSplitFactors.size());
	}

	inline bool DirectionalLight::IsUsingFixedShadowCascadeSplit() const
	{
		return m_shadowCascadeFixedSplit;
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

	inline void DirectionalLight::UpdateShadowCascadeFixedSplitFactors(std::span<const float> splitFactors)
	{
		m_fixedSplitFactors.clear();
		for (float factor : splitFactors)
			m_fixedSplitFactors.push_back(factor);
	}

	inline void DirectionalLight::UpdateShadowCascadeSplitLambda(float lambda)
	{
		m_shadowCascadeSplitLambda = lambda;
	}

	inline void DirectionalLight::UpdateBoundingVolume()
	{
		Light::UpdateBoundingVolume(BoundingVolumef::Infinite()); //< will trigger OnLightDataInvalided
	}
}
