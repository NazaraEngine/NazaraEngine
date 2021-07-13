// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORWARDFRAMEPIPELINE_HPP
#define NAZARA_FORWARDFRAMEPIPELINE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <optional>
#include <unordered_map>
#include <unordered_set>

namespace Nz
{
	class NAZARA_GRAPHICS_API ForwardFramePipeline : public FramePipeline
	{
		public:
			ForwardFramePipeline();
			ForwardFramePipeline(const ForwardFramePipeline&) = delete;
			ForwardFramePipeline(ForwardFramePipeline&&) = delete;
			~ForwardFramePipeline() = default;

			void InvalidateViewer(AbstractViewer* viewerInstance) override;
			void InvalidateWorldInstance(WorldInstance* worldInstance) override;

			void RegisterInstancedDrawable(WorldInstance* worldInstance, const InstancedRenderable* instancedRenderable) override;
			void RegisterViewer(AbstractViewer* viewerInstance) override;

			void Render(RenderFrame& renderFrame) override;

			void UnregisterInstancedDrawable(WorldInstance* worldInstance, const InstancedRenderable* instancedRenderable) override;
			void UnregisterViewer(AbstractViewer* viewerInstance) override;

			ForwardFramePipeline& operator=(const ForwardFramePipeline&) = delete;
			ForwardFramePipeline& operator=(ForwardFramePipeline&&) = delete;

		private:
			BakedFrameGraph BuildFrameGraph();
			void RegisterMaterial(MaterialPass* material);
			void UnregisterMaterial(MaterialPass* material);

			struct MaterialData
			{
				std::size_t usedCount = 0;

				NazaraSlot(MaterialPass, OnMaterialInvalidated, onMaterialInvalided);
			};

			struct RenderableData
			{
				NazaraSlot(InstancedRenderable, OnMaterialInvalidated, onMaterialInvalidated);
			};

			struct ViewerData
			{
				std::size_t colorAttachment;
				std::size_t depthStencilAttachment;
				ShaderBindingPtr blitShaderBinding;
			};

			std::size_t m_forwardPass;
			std::unordered_map<AbstractViewer*, ViewerData> m_viewers;
			std::unordered_map<MaterialPass*, MaterialData> m_materials;
			std::unordered_map<WorldInstance*, std::unordered_map<const InstancedRenderable*, RenderableData>> m_renderables;
			std::unordered_set<AbstractViewer*> m_invalidatedViewerInstances;
			std::unordered_set<MaterialPass*> m_invalidatedMaterials;
			std::unordered_set<WorldInstance*> m_invalidatedWorldInstances;
			BakedFrameGraph m_bakedFrameGraph;
			bool m_rebuildFrameGraph;
			bool m_rebuildForwardPass;
	};
}

#include <Nazara/Graphics/ForwardFramePipeline.inl>

#endif
