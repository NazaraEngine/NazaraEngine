// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SPOTLIGHT_HPP
#define NAZARA_GRAPHICS_SPOTLIGHT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Math/Angle.hpp>
#include <memory>

namespace Nz
{
	class NAZARA_GRAPHICS_API SpotLight : public Light
	{
		public:
			SpotLight();
			SpotLight(const SpotLight&) = delete;
			SpotLight(SpotLight&&) noexcept = default;
			~SpotLight() = default;

			float ComputeContributionScore(const BoundingVolumef& boundingVolume) const override;

			void FillLightData(void* data) const override;

			inline float GetAmbientFactor() const;
			inline float GetDiffuseFactor() const;
			inline Color GetColor() const;
			inline const Vector3f& GetDirection() const;
			inline RadianAnglef GetInnerAngle() const;
			inline RadianAnglef GetOuterAngle() const;
			inline const Vector3f& GetPosition() const;
			inline const Quaternionf& GetRotation() const;
			inline float GetRadius() const;

			inline void UpdateAmbientFactor(float factor);
			inline void UpdateColor(Color color);
			inline void UpdateDiffuseFactor(float factor);
			inline void UpdateDirection(const Vector3f& direction);
			inline void UpdateInnerAngle(RadianAnglef innerAngle);
			inline void UpdateOuterAngle(RadianAnglef outerAngle);
			inline void UpdatePosition(const Vector3f& position);
			inline void UpdateRadius(float radius);
			inline void UpdateRotation(const Quaternionf& rotation);

			void UpdateTransform(const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale) override;

			SpotLight& operator=(const SpotLight&) = delete;
			SpotLight& operator=(SpotLight&&) noexcept = default;

		private:
			inline void UpdateBoundingVolume();
			inline void UpdateViewProjMatrix();

			Color m_color;
			Quaternionf m_rotation;
			Matrix4f m_viewProjMatrix;
			RadianAnglef m_innerAngle;
			RadianAnglef m_outerAngle;
			Vector3f m_direction;
			Vector3f m_position;
			float m_ambientFactor;
			float m_diffuseFactor;
			float m_invRadius;
			float m_radius;
			float m_innerAngleCos;
			float m_outerAngleCos;
			float m_outerAngleTan;
	};
}

#include <Nazara/Graphics/SpotLight.inl>

#endif // NAZARA_GRAPHICS_SPOTLIGHT_HPP
