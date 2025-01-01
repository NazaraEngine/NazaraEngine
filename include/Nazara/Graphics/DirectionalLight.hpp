// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_DIRECTIONALLIGHT_HPP
#define NAZARA_GRAPHICS_DIRECTIONALLIGHT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <memory>
#include <span>

namespace Nz
{
	class NAZARA_GRAPHICS_API DirectionalLight : public Light
	{
		public:
			DirectionalLight();
			DirectionalLight(const DirectionalLight&) = delete;
			DirectionalLight(DirectionalLight&&) noexcept = default;
			~DirectionalLight() = default;

			float ComputeContributionScore(const Frustumf& viewerFrustum) const override;

			inline void EnableFixedShadowCascadSplit(bool enable);

			bool FrustumCull(const Frustumf& viewerFrustum) const override;

			std::unique_ptr<LightShadowData> InstanciateShadowData(FramePipeline& pipeline, ElementRendererRegistry& elementRegistry) const override;

			inline float GetAmbientFactor() const;
			inline float GetDiffuseFactor() const;
			inline Color GetColor() const;
			inline const Vector3f& GetDirection() const;
			inline const Quaternionf& GetRotation() const;
			inline float GetShadowCascadeSplitLambda() const;
			inline std::span<const float> GetShadowCascadeFixedSplitFactors() const;

			inline bool IsUsingFixedShadowCascadeSplit() const;

			inline void UpdateAmbientFactor(float factor);
			inline void UpdateColor(Color color);
			inline void UpdateDiffuseFactor(float factor);
			inline void UpdateDirection(const Vector3f& direction);
			inline void UpdateRotation(const Quaternionf& rotation);
			inline void UpdateShadowCascadeFixedSplitFactors(std::span<const float> splitFactors);
			inline void UpdateShadowCascadeSplitLambda(float lambda);

			void UpdateTransform(const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale) override;

			DirectionalLight& operator=(const DirectionalLight&) = delete;
			DirectionalLight& operator=(DirectionalLight&&) noexcept = default;

		private:
			inline void UpdateBoundingVolume();

			Color m_color;
			HybridVector<float, 8> m_fixedSplitFactors;
			Quaternionf m_rotation;
			Vector3f m_direction;
			float m_ambientFactor;
			float m_diffuseFactor;
			float m_shadowCascadeSplitLambda;
			bool m_shadowCascadeFixedSplit;
	};
}

#include <Nazara/Graphics/DirectionalLight.inl>

#endif // NAZARA_GRAPHICS_DIRECTIONALLIGHT_HPP
