// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_DIRECTIONALLIGHTSHADOWDATA_HPP
#define NAZARA_GRAPHICS_DIRECTIONALLIGHTSHADOWDATA_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/LightShadowData.hpp>
#include <Nazara/Graphics/ShadowViewer.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <NazaraUtils/SparsePtr.hpp>
#include <unordered_map>
#include <vector>

namespace Nz
{
	class FramePipeline;
	class DirectionalLight;

	class NAZARA_GRAPHICS_API DirectionalLightShadowData : public LightShadowData
	{
		public:
			DirectionalLightShadowData(FramePipeline& pipeline, const DirectionalLight& light, std::size_t cascadeCount);
			DirectionalLightShadowData(const DirectionalLightShadowData&) = delete;
			DirectionalLightShadowData(DirectionalLightShadowData&&) = delete;
			~DirectionalLightShadowData();

			inline void EnableShadowStabilization(bool enable);

			void ForEachView(FunctionRef<void(std::size_t shadowAtlasEntry, ShadowViewer& shadowViewer)> callback) override;

			inline std::size_t GetCascadeCount() const;
			inline void GetCascadeData(const AbstractViewer* viewer, SparsePtr<float> distance, SparsePtr<Matrix4f> viewProjMatrix) const;
			inline float GetDepthPlaneFactor() const;

			inline bool IsShadowStabilizationEnabled() const;

			void PrepareRendering(RenderResources& renderResources) override;

			void RegisterToAtlas(ShadowAtlas& atlas) override;
			void RegisterViewer(const AbstractViewer* viewer) override;

			inline void SetDepthPlaneFactor(float depthPlaneFactor);

			void UnregisterViewer(const AbstractViewer* viewer) override;

			void WriteToShader(const ShadowAtlas& atlas, const AbstractViewer* viewer, void* basePtr) const override;

			DirectionalLightShadowData& operator=(const DirectionalLightShadowData&) = delete;
			DirectionalLightShadowData& operator=(DirectionalLightShadowData&&) = delete;

		private:
			struct CascadeData;

			template<typename F> void ForEachCascade(F&& callback);

			void ComputeLightView(CascadeData& cascade, const Frustumf& cascadeFrustum, float cascadeDist);
			void StabilizeShadows(CascadeData& cascade);

			NazaraSlot(Light, OnLightShadowMapSettingChange, m_onLightShadowMapSettingChange);
			NazaraSlot(Light, OnLightTransformInvalidated, m_onLightTransformInvalidated);

			struct CascadeData
			{
				std::size_t attachmentIndex;
				Matrix4f viewProjMatrix;
				ShadowViewer viewer;
				float distance;
			};

			struct PerViewerData
			{
				FixedVector<CascadeData, 8> cascades;
				std::size_t firstShadowAtlasIndex;
				std::size_t textureArrayAttachmentIndex;
			};

			std::size_t m_cascadeCount;
			std::vector<std::unique_ptr<PerViewerData>> m_destructionQueue;
			std::unordered_map<const AbstractViewer*, std::unique_ptr<PerViewerData>> m_viewerData;
			FramePipeline& m_pipeline;
			const DirectionalLight& m_light;
			bool m_isShadowStabilizationEnabled;
			float m_depthPlaneFactor;
			float m_invTexelScale;
			float m_texelScale;
	};
}

#include <Nazara/Graphics/DirectionalLightShadowData.inl>

#endif // NAZARA_GRAPHICS_DIRECTIONALLIGHTSHADOWDATA_HPP
