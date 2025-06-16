// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FORWARDPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_FORWARDPIPELINEPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderElementOwner.hpp>
#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Renderer/UploadPool.hpp>

namespace Nz
{
	class AbstractViewer;
	class DirectionalLight;
	class ElementRendererRegistry;
	class FrameGraph;
	class FramePass;
	class FramePipeline;
	class Light;
	class PointLight;
	class SpotLight;

	class NAZARA_GRAPHICS_API ForwardPipelinePass : public FramePipelinePass, TransferInterface
	{
		public:
			ForwardPipelinePass(PassData& passData, std::string passName, const ParameterList& parameters = {});
			ForwardPipelinePass(const ForwardPipelinePass&) = delete;
			ForwardPipelinePass(ForwardPipelinePass&&) = delete;
			~ForwardPipelinePass() = default;

			inline void InvalidateCommandBuffers();
			void InvalidateElements() override;

			void Prepare(FrameData& frameData) override;

			void RegisterMaterialInstance(const MaterialInstance& material) override;

			FramePass& RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs) override;

			void UnregisterMaterialInstance(const MaterialInstance& material) override;

			ForwardPipelinePass& operator=(const ForwardPipelinePass&) = delete;
			ForwardPipelinePass& operator=(ForwardPipelinePass&&) = delete;

		private:
			void OnTransfer(RenderResources& renderResources, CommandBufferBuilder& builder) override;

			void PrepareDirectionalLights(void* lightMemory);
			void PreparePointLights(void* lightMemory);
			void PrepareSpotLights(void* lightMemory);
			void PrepareLights(RenderResources& renderResources, const Frustumf& frustum, const Bitset<UInt64>& visibleLights);

			struct MaterialPassEntry
			{
				std::size_t usedCount = 1;

				NazaraSlot(MaterialInstance, OnMaterialInstancePipelineInvalidated, onMaterialInstancePipelineInvalidated);
				NazaraSlot(MaterialInstance, OnMaterialInstanceShaderBindingInvalidated, onMaterialInstanceShaderBindingInvalidated);
			};

			template<typename T>
			struct RenderableLight
			{
				const T* light;
				std::size_t lightIndex;
				float contributionScore;
			};

			std::size_t m_forwardPassIndex;
			std::size_t m_lastVisibilityHash;
			std::shared_ptr<RenderBuffer> m_lightDataBuffer;
			std::string m_passName;
			std::vector<std::unique_ptr<ElementRendererData>> m_elementRendererData;
			std::vector<RenderElementOwner> m_renderElements;
			std::unordered_map<const MaterialInstance*, MaterialPassEntry> m_materialInstances;
			std::vector<RenderableLight<DirectionalLight>> m_directionalLights;
			std::vector<RenderableLight<PointLight>> m_pointLights;
			std::vector<RenderableLight<SpotLight>> m_spotLights;
			ElementRenderer::RenderStates m_renderState;
			RenderQueue<const RenderElement*> m_renderQueue;
			RenderQueueRegistry m_renderQueueRegistry;
			AbstractViewer* m_viewer;
			ElementRendererRegistry& m_elementRegistry;
			FramePipeline& m_pipeline;
			UploadPool::Allocation* m_pendingLightUploadAllocation;
			UInt32 m_renderMask;
			bool m_handleLights;
			bool m_rebuildCommandBuffer;
			bool m_rebuildElements;
	};
}

#include <Nazara/Graphics/ForwardPipelinePass.inl>

#endif // NAZARA_GRAPHICS_FORWARDPIPELINEPASS_HPP
