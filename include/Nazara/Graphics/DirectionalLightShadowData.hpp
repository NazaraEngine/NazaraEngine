// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_DIRECTIONALLIGHTSHADOWDATA_HPP
#define NAZARA_GRAPHICS_DIRECTIONALLIGHTSHADOWDATA_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/DepthPipelinePass.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/LightShadowData.hpp>
#include <Nazara/Graphics/ShadowViewer.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <NazaraUtils/SparsePtr.hpp>
#include <unordered_map>

namespace Nz
{
	class FramePipeline;
	class DirectionalLight;

	class NAZARA_GRAPHICS_API DirectionalLightShadowData : public LightShadowData
	{
		public:
			DirectionalLightShadowData(FramePipeline& pipeline, ElementRendererRegistry& elementRegistry, const DirectionalLight& light, std::size_t cascadeCount);
			DirectionalLightShadowData(const DirectionalLightShadowData&) = delete;
			DirectionalLightShadowData(DirectionalLightShadowData&&) = delete;
			~DirectionalLightShadowData() = default;

			inline void EnableShadowStabilization(bool enable);

			inline std::size_t GetCascadeCount() const;
			inline void GetCascadeData(const AbstractViewer* viewer, SparsePtr<float> distance, SparsePtr<Matrix4f> viewProjMatrix) const;

			inline bool IsShadowStabilization() const;

			void PrepareRendering(RenderResources& renderResources, const AbstractViewer* viewer) override;

			void RegisterMaterialInstance(const MaterialInstance& matInstance) override;
			void RegisterPassInputs(FramePass& pass, const AbstractViewer* viewer) override;
			void RegisterToFrameGraph(FrameGraph& frameGraph, const AbstractViewer* viewer) override;
			void RegisterViewer(const AbstractViewer* viewer) override;

			const Texture* RetrieveLightShadowmap(const BakedFrameGraph& bakedGraph, const AbstractViewer* viewer) const override;

			void UnregisterMaterialInstance(const MaterialInstance& matInstance) override;
			void UnregisterViewer(const AbstractViewer* viewer) override;

			DirectionalLightShadowData& operator=(const DirectionalLightShadowData&) = delete;
			DirectionalLightShadowData& operator=(DirectionalLightShadowData&&) = delete;

		private:
			struct CascadeData;

			template<typename F> void ForEachCascade(F&& callback);

			void ComputeLightView(CascadeData& cascade, const Frustumf& cascadeFrustum, float cascadeDist);
			void StabilizeShadows(CascadeData& cascade);

			NazaraSlot(Light, OnLightShadowMapSettingChange, m_onLightShadowMapSettingChange);
			NazaraSlot(Light, OnLightTransformInvalided, m_onLightTransformInvalidated);

			struct CascadeData
			{
				std::optional<DepthPipelinePass> depthPass;
				std::size_t attachmentIndex;
				Matrix4f viewProjMatrix;
				ShadowViewer viewer;
				float distance;
			};

			struct PerViewerData
			{
				FixedVector<CascadeData, 8> cascades;
				std::size_t textureArrayAttachmentIndex;
			};

			std::size_t m_cascadeCount;
			std::unordered_map<const AbstractViewer*, std::unique_ptr<PerViewerData>> m_viewerData;
			ElementRendererRegistry& m_elementRegistry;
			FramePipeline& m_pipeline;
			const DirectionalLight& m_light;
			bool m_isShadowStabilizationEnabled;
			float m_invTexelScale;
			float m_texelScale;
	};
}

#include <Nazara/Graphics/DirectionalLightShadowData.inl>

#endif // NAZARA_GRAPHICS_DIRECTIONALLIGHTSHADOWDATA_HPP
