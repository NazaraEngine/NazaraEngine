// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FORWARDFRAMEPIPELINE_HPP
#define NAZARA_GRAPHICS_FORWARDFRAMEPIPELINE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/DepthPipelinePass.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/ForwardPipelinePass.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Nz
{
	class PointLight;
	class RenderFrame;
	class RenderTarget;

	class NAZARA_GRAPHICS_API ForwardFramePipeline : public FramePipeline
	{
		public:
			ForwardFramePipeline();
			ForwardFramePipeline(const ForwardFramePipeline&) = delete;
			ForwardFramePipeline(ForwardFramePipeline&&) = delete;
			~ForwardFramePipeline();

			void InvalidateViewer(AbstractViewer* viewerInstance) override;
			void InvalidateWorldInstance(WorldInstance* worldInstance) override;

			void RegisterInstancedDrawable(WorldInstancePtr worldInstance, const InstancedRenderable* instancedRenderable, UInt32 renderMask) override;
			void RegisterLight(std::shared_ptr<Light> light, UInt32 renderMask) override;
			void RegisterMaterialPass(MaterialPass* materialPass) override;
			void RegisterViewer(AbstractViewer* viewerInstance, Int32 renderOrder) override;

			void Render(RenderFrame& renderFrame) override;

			void UnregisterInstancedDrawable(const WorldInstancePtr& worldInstance, const InstancedRenderable* instancedRenderable) override;
			void UnregisterLight(Light* light) override;
			void UnregisterMaterialPass(MaterialPass* material) override;
			void UnregisterViewer(AbstractViewer* viewerInstance) override;

			ForwardFramePipeline& operator=(const ForwardFramePipeline&) = delete;
			ForwardFramePipeline& operator=(ForwardFramePipeline&&) = delete;

		private:
			BakedFrameGraph BuildFrameGraph();

			struct ViewerData;

			struct LightData
			{
				std::shared_ptr<Light> light;
				UInt32 renderMask;

				NazaraSlot(Light, OnLightDataInvalided, onLightInvalidated);
			};

			struct MaterialPassData
			{
				std::size_t usedCount = 0;

				NazaraSlot(MaterialPass, OnMaterialPassInvalidated, onMaterialPassInvalided);
			};

			struct RenderableData
			{
				UInt32 renderMask = 0;

				NazaraSlot(InstancedRenderable, OnElementInvalidated, onElementInvalidated);
				NazaraSlot(InstancedRenderable, OnMaterialInvalidated, onMaterialInvalidated);
			};

			struct RenderTargetData
			{
				std::size_t finalAttachment;
				std::vector<const ViewerData*> viewers;
				ShaderBindingPtr blitShaderBinding;
			};

			struct ViewerData
			{
				std::size_t colorAttachment;
				std::size_t depthStencilAttachment;
				std::unique_ptr<DepthPipelinePass> depthPrepass;
				std::unique_ptr<ForwardPipelinePass> forwardPass;
				Int32 renderOrder = 0;
				RenderQueueRegistry forwardRegistry;
				RenderQueue<RenderElement*> forwardRenderQueue;
				ShaderBindingPtr blitShaderBinding;
			};

			std::size_t m_forwardPassIndex;
			std::unordered_map<AbstractViewer*, ViewerData> m_viewers;
			std::unordered_map<Light*, LightData> m_lights;
			std::unordered_map<MaterialPass*, MaterialPassData> m_activeMaterialPasses;
			std::unordered_map<WorldInstancePtr, std::unordered_map<const InstancedRenderable*, RenderableData>> m_renderables;
			std::unordered_map<const RenderTarget*, RenderTargetData> m_renderTargets;
			std::unordered_set<AbstractViewer*> m_invalidatedViewerInstances;
			std::unordered_set<MaterialPass*> m_invalidatedMaterialPasses;
			std::unordered_set<WorldInstance*> m_invalidatedWorldInstances;
			std::unordered_set<WorldInstancePtr> m_removedWorldInstances;
			std::vector<ElementRenderer::RenderStates> m_renderStates;
			std::vector<FramePipelinePass::VisibleRenderable> m_visibleRenderables;
			std::vector<const Light*> m_visibleLights;
			BakedFrameGraph m_bakedFrameGraph;
			RenderFrame* m_currentRenderFrame;
			bool m_rebuildFrameGraph;
	};
}

#include <Nazara/Graphics/ForwardFramePipeline.inl>

#endif // NAZARA_GRAPHICS_FORWARDFRAMEPIPELINE_HPP
