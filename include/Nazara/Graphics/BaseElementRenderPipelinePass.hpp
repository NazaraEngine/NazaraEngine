// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_BASEELEMENTRENDERPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_BASEELEMENTRENDERPIPELINEPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
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

	class NAZARA_GRAPHICS_API BaseElementRenderPipelinePass : public FramePipelinePass
	{
		public:
			BaseElementRenderPipelinePass(PassData& passData);
			BaseElementRenderPipelinePass(const BaseElementRenderPipelinePass&) = delete;
			BaseElementRenderPipelinePass(BaseElementRenderPipelinePass&&) = delete;
			~BaseElementRenderPipelinePass() = default;

			void ClearRenderables() override;

			void Prepare(FrameData& frameData) override;

			void RegisterMaterialInstance(const MaterialInstance& material) override;
			void RegisterRenderable(std::size_t renderableIndex, UInt32 instanceIndex, const InstancedRenderable& instancedRenderable, const SkeletonInstance* skeletonInstance, UInt32 renderMask, const Recti& scissorBox) override;

			void UnregisterMaterialInstance(const MaterialInstance& material) override;
			void UnregisterRenderable(std::size_t renderableIndex) override;

			BaseElementRenderPipelinePass& operator=(const BaseElementRenderPipelinePass&) = delete;
			BaseElementRenderPipelinePass& operator=(BaseElementRenderPipelinePass&&) = delete;

		protected:
			struct MaterialPassEntry
			{
				std::size_t usedCount = 1;

				NazaraSlot(MaterialInstance, OnMaterialInstancePipelineInvalidated, onMaterialInstancePipelineInvalidated);
			};

			struct RenderElementIndices
			{
				std::size_t first;
				std::size_t count;
			};

			std::size_t m_passIndex;
			std::unordered_map<const MaterialInstance*, MaterialPassEntry> m_materialInstances;
			std::unordered_map<std::size_t /*renderableIndex*/, RenderElementIndices> m_renderElementsIndices;
			std::vector<std::unique_ptr<ElementRendererData>> m_elementRendererData;
			std::vector<RenderElementOwner> m_renderElements;
			std::vector<RenderElementOwner> m_deletedRenderElements;
			RenderQueue<const RenderElement*> m_renderQueue;
			RenderQueueRegistry m_renderQueueRegistry;
			ElementRendererRegistry& m_elementRegistry;
			UInt32 m_renderMask;
			bool m_rebuildRenderQueue;
	};
}

#include <Nazara/Graphics/BaseElementRenderPipelinePass.inl>

#endif // NAZARA_GRAPHICS_BASEELEMENTRENDERPIPELINEPASS_HPP
