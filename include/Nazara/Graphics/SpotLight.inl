// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/SpotLight.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline SpotLight::SpotLight() :
	Light(SafeCast<UInt8>(BasicLightType::Spot)),
	m_color(Color::White),
	m_position(Vector3f::Zero()),
	m_ambientFactor(0.2f),
	m_diffuseFactor(1.f)
	{
		UpdateInnerAngle(DegreeAnglef(30.f));
		UpdateOuterAngle(DegreeAnglef(45.f));
		UpdateRadius(5.f);
		UpdateRotation(Quaternionf::Identity());
	}

	inline float SpotLight::GetAmbientFactor() const
	{
		return m_ambientFactor;
	}

	inline Color SpotLight::GetColor() const
	{
		return m_color;
	}

	inline const Vector3f& SpotLight::GetDirection() const
	{
		return m_direction;
	}

	inline RadianAnglef SpotLight::GetInnerAngle() const
	{
		return m_innerAngle;
	}

	inline RadianAnglef SpotLight::GetOuterAngle() const
	{
		return m_outerAngle;
	}

	inline const Vector3f& SpotLight::GetPosition() const
	{
		return m_position;
	}

	inline const Quaternionf& SpotLight::GetRotation() const
	{
		return m_rotation;
	}

	inline float SpotLight::GetDiffuseFactor() const
	{
		return m_diffuseFactor;
	}

	inline float SpotLight::GetRadius() const
	{
		return m_radius;
	}

	inline void SpotLight::UpdateAmbientFactor(float factor)
	{
		m_ambientFactor = factor;

		OnLightDataInvalided(this);
	}

	inline void SpotLight::UpdateColor(Color color)
	{
		m_color = color;

		OnLightDataInvalided(this);
	}

	inline void SpotLight::UpdateDiffuseFactor(float factor)
	{
		m_diffuseFactor = factor;

		OnLightDataInvalided(this);
	}

	inline void SpotLight::UpdateDirection(const Vector3f& direction)
	{
		UpdateRotation(Quaternionf::RotationBetween(Vector3f::Forward(), direction));
	}

	inline void SpotLight::UpdateInnerAngle(RadianAnglef innerAngle)
	{
		m_innerAngle = innerAngle;
		m_innerAngleCos = m_innerAngle.GetCos();
	}

	inline void SpotLight::UpdateOuterAngle(RadianAnglef outerAngle)
	{
		m_outerAngle = outerAngle;
		m_outerAngleCos = m_outerAngle.GetCos();
		m_outerAngleTan = m_outerAngle.GetTan();

		UpdateBoundingVolume();
		UpdateViewProjMatrix();
	}

	inline void SpotLight::UpdatePosition(const Vector3f& position)
	{
		m_position = position;

		UpdateBoundingVolume();
		UpdateViewProjMatrix();
		OnLightTransformInvalided(this);
	}

	inline void SpotLight::UpdateRadius(float radius)
	{
		m_radius = radius;
		m_invRadius = 1.f / m_radius;

		UpdateBoundingVolume();
		UpdateViewProjMatrix();
	}

	inline void SpotLight::UpdateRotation(const Quaternionf& rotation)
	{
		m_rotation = rotation;
		m_direction = rotation * Vector3f::Forward();

		UpdateBoundingVolume();
		UpdateViewProjMatrix();
		OnLightTransformInvalided(this);
	}

	inline void SpotLight::UpdateBoundingVolume()
	{
		// We make a box center in the origin
		Boxf box(Vector3f::Zero());

		// We compute the other points
		Vector3f base(Vector3f::Forward() * m_radius);

		// Now we need the radius of the projected circle depending on the distance
		// Tangent = Opposite/Adjacent <=> Opposite = Adjacent * Tangent
		float radius = m_radius * m_outerAngleTan;
		Vector3f lExtend = Vector3f::Left() * radius;
		Vector3f uExtend = Vector3f::Up() * radius;

		// And we add the four extremities of our pyramid
		box.ExtendTo(base + lExtend + uExtend);
		box.ExtendTo(base + lExtend - uExtend);
		box.ExtendTo(base - lExtend + uExtend);
		box.ExtendTo(base - lExtend - uExtend);

		BoundingVolumef boundingVolume(box);
		boundingVolume.Update(Matrix4f::Transform(m_position, m_rotation));

		Light::UpdateBoundingVolume(boundingVolume); //< will trigger OnLightDataInvalided
	}

	inline void SpotLight::UpdateViewProjMatrix()
	{
		Matrix4f biasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
		                    0.0f, 0.5f, 0.0f, 0.0f,
		                    0.0f, 0.0f, 1.0f, 0.0f,
		                    0.5f, 0.5f, 0.0f, 1.0f);

		Matrix4f projection = Matrix4f::Perspective(m_outerAngle * 2.f, 1.f, 1.f, m_radius);
		Matrix4f view = Matrix4f::TransformInverse(m_position, m_rotation);

		m_viewProjMatrix = view * projection * biasMatrix;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
