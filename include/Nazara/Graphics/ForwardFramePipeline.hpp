// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FORWARDFRAMEPIPELINE_HPP
#define NAZARA_GRAPHICS_FORWARDFRAMEPIPELINE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Nz
{
	class RenderFrame;
	class RenderTarget;

	class NAZARA_GRAPHICS_API ForwardFramePipeline : public FramePipeline
	{
		public:
			ForwardFramePipeline();
			ForwardFramePipeline(const ForwardFramePipeline&) = delete;
			ForwardFramePipeline(ForwardFramePipeline&&) = delete;
			~ForwardFramePipeline() = default;

			void InvalidateViewer(AbstractViewer* viewerInstance) override;
			void InvalidateWorldInstance(WorldInstance* worldInstance) override;

			void RegisterInstancedDrawable(WorldInstancePtr worldInstance, const InstancedRenderable* instancedRenderable, UInt32 renderMask) override;
			void RegisterViewer(AbstractViewer* viewerInstance) override;

			void Render(RenderFrame& renderFrame) override;

			void UnregisterInstancedDrawable(const WorldInstancePtr& worldInstance, const InstancedRenderable* instancedRenderable) override;
			void UnregisterViewer(AbstractViewer* viewerInstance) override;

			ForwardFramePipeline& operator=(const ForwardFramePipeline&) = delete;
			ForwardFramePipeline& operator=(ForwardFramePipeline&&) = delete;

		private:
			BakedFrameGraph BuildFrameGraph();

			void RegisterMaterialPass(MaterialPass* material);
			template<typename F> void ProcessRenderQueue(const RenderQueue<RenderElement*>& renderQueue, F&& callback);
			void UnregisterMaterialPass(MaterialPass* material);

			struct MaterialData
			{
				std::size_t usedCount = 0;

				NazaraSlot(MaterialPass, OnMaterialInvalidated, onMaterialInvalided);
			};

			struct RenderableData
			{
				UInt32 renderMask = 0;

				NazaraSlot(InstancedRenderable, OnElementInvalidated, onElementInvalidated);
				NazaraSlot(InstancedRenderable, OnMaterialInvalidated, onMaterialInvalidated);
			};

			struct VisibleRenderable
			{
				const InstancedRenderable* instancedRenderable;
				const WorldInstance* worldInstance;
			};

			struct ViewerData
			{
				std::size_t colorAttachment;
				std::size_t depthStencilAttachment;
				std::size_t visibilityHash = 0;
				std::vector<std::unique_ptr<RenderElement>> depthPrepassRenderElements;
				std::vector<std::unique_ptr<RenderElement>> forwardRenderElements;
				std::vector<std::unique_ptr<ElementRendererData>> elementRendererData;
				RenderQueueRegistry depthPrepassRegistry;
				RenderQueueRegistry forwardRegistry;
				RenderQueue<RenderElement*> depthPrepassRenderQueue;
				RenderQueue<RenderElement*> forwardRenderQueue;
				ShaderBindingPtr blitShaderBinding;
				bool prepare = true;
				bool rebuildDepthPrepass = true;
				bool rebuildForwardPass = true;
			};

			std::size_t m_depthPassIndex;
			std::size_t m_forwardPassIndex;
			std::unordered_map<AbstractViewer*, ViewerData> m_viewers;
			std::unordered_map<MaterialPass*, MaterialData> m_materials;
			std::unordered_map<WorldInstancePtr, std::unordered_map<const InstancedRenderable*, RenderableData>> m_renderables;
			std::unordered_map<const RenderTarget*, std::vector<const ViewerData*>> m_viewerPerTarget;
			std::unordered_set<AbstractViewer*> m_invalidatedViewerInstances;
			std::unordered_set<MaterialPass*> m_invalidatedMaterials;
			std::unordered_set<WorldInstance*> m_invalidatedWorldInstances;
			std::unordered_set<WorldInstancePtr> m_removedWorldInstances;
			std::vector<std::unique_ptr<ElementRenderer>> m_elementRenderers;
			std::vector<VisibleRenderable> m_visibleRenderables;
			BakedFrameGraph m_bakedFrameGraph;
			RenderFrame* m_currentRenderFrame;
			bool m_rebuildFrameGraph;
	};
}

#include <Nazara/Graphics/ForwardFramePipeline.inl>

#endif // NAZARA_GRAPHICS_FORWARDFRAMEPIPELINE_HPP
