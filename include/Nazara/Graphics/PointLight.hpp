// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_POINTLIGHT_HPP
#define NAZARA_GRAPHICS_POINTLIGHT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <memory>

namespace Nz
{
	class NAZARA_GRAPHICS_API PointLight : public Light
	{
		public:
			PointLight();
			PointLight(const PointLight&) = delete;
			PointLight(PointLight&&) noexcept = default;
			~PointLight() = default;

			float ComputeContributionScore(const BoundingVolumef& boundingVolume) const override;

			void FillLightData(void* data) const override;

			std::unique_ptr<LightShadowData> InstanciateShadowData(FramePipeline& pipeline, ElementRendererRegistry& elementRegistry) const override;

			inline float GetAmbientFactor() const;
			inline float GetDiffuseFactor() const;
			inline Color GetColor() const;
			inline const Vector3f& GetPosition() const;
			inline float GetRadius() const;

			inline void UpdateAmbientFactor(float factor);
			inline void UpdateColor(Color color);
			inline void UpdateDiffuseFactor(float factor);
			inline void UpdatePosition(const Vector3f& position);
			inline void UpdateRadius(float radius);

			void UpdateTransform(const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale) override;

			PointLight& operator=(const PointLight&) = delete;
			PointLight& operator=(PointLight&&) noexcept = default;

		private:
			inline void UpdateBoundingVolume();

			Color m_color;
			Vector3f m_position;
			float m_ambientFactor;
			float m_diffuseFactor;
			float m_invRadius;
			float m_radius;
	};
}

#include <Nazara/Graphics/PointLight.inl>

#endif // NAZARA_GRAPHICS_POINTLIGHT_HPP
