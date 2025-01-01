// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RASTERPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_RASTERPIPELINEPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderElementOwner.hpp>
#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <Nazara/Math/Frustum.hpp>

namespace Nz
{
	class AbstractViewer;
	class ElementRendererRegistry;
	class FrameGraph;
	class FramePass;
	class FramePipeline;

	class NAZARA_GRAPHICS_API RasterPipelinePass : public FramePipelinePass
	{
		public:
			inline RasterPipelinePass(PassData& passData, std::string passName, const ParameterList& parameters);
			inline RasterPipelinePass(PassData& passData, std::string passName, std::size_t materialPassIndex);
			RasterPipelinePass(const RasterPipelinePass&) = delete;
			RasterPipelinePass(RasterPipelinePass&&) = delete;
			~RasterPipelinePass() = default;

			inline void InvalidateCommandBuffers();
			void InvalidateElements() override;

			void Prepare(FrameData& frameData) override;

			void RegisterMaterialInstance(const MaterialInstance& materialInstance) override;

			FramePass& RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs) override;

			void UnregisterMaterialInstance(const MaterialInstance& materialInstance) override;

			RasterPipelinePass& operator=(const RasterPipelinePass&) = delete;
			RasterPipelinePass& operator=(RasterPipelinePass&&) = delete;

			static std::size_t GetMaterialPassIndex(const ParameterList& parameters);

		private:
			struct MaterialPassEntry
			{
				std::size_t usedCount = 1;

				NazaraSlot(MaterialInstance, OnMaterialInstancePipelineInvalidated, onMaterialInstancePipelineInvalidated);
				NazaraSlot(MaterialInstance, OnMaterialInstanceShaderBindingInvalidated, onMaterialInstanceShaderBindingInvalidated);
			};

			std::size_t m_passIndex;
			std::size_t m_lastVisibilityHash;
			std::string m_passName;
			std::vector<std::unique_ptr<ElementRendererData>> m_elementRendererData;
			std::vector<RenderElementOwner> m_renderElements;
			std::unordered_map<const MaterialInstance*, MaterialPassEntry> m_materialInstances;
			RenderQueue<const RenderElement*> m_renderQueue;
			RenderQueueRegistry m_renderQueueRegistry;
			AbstractViewer* m_viewer;
			ElementRendererRegistry& m_elementRegistry;
			FramePipeline& m_pipeline;
			bool m_rebuildCommandBuffer;
			bool m_rebuildElements;
	};
}

#include <Nazara/Graphics/RasterPipelinePass.inl>

#endif // NAZARA_GRAPHICS_RASTERPIPELINEPASS_HPP
