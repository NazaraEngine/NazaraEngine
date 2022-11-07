// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DepthPipelinePass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/ElementRendererRegistry.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	DepthPipelinePass::DepthPipelinePass(FramePipeline& owner, ElementRendererRegistry& elementRegistry, AbstractViewer* viewer) :
	m_lastVisibilityHash(0),
	m_viewer(viewer),
	m_elementRegistry(elementRegistry),
	m_pipeline(owner),
	m_rebuildCommandBuffer(false),
	m_rebuildElements(false)
	{
		m_depthPassIndex = Graphics::Instance()->GetMaterialPassRegistry().GetPassIndex("DepthPass");
	}

	void DepthPipelinePass::Prepare(RenderFrame& renderFrame, const Frustumf& frustum, const std::vector<FramePipelinePass::VisibleRenderable>& visibleRenderables, std::size_t visibilityHash)
	{
		if (m_lastVisibilityHash != visibilityHash || m_rebuildElements) //< FIXME
		{
			renderFrame.PushForRelease(std::move(m_renderElements));
			m_renderElements.clear();

			for (const auto& renderableData : visibleRenderables)
			{
				InstancedRenderable::ElementData elementData{
					&renderableData.scissorBox,
					renderableData.skeletonInstance,
					renderableData.worldInstance
				};

				renderableData.instancedRenderable->BuildElement(m_elementRegistry, elementData, m_depthPassIndex, m_renderElements);
			}

			m_renderQueueRegistry.Clear();
			m_renderQueue.Clear();

			for (const auto& renderElement : m_renderElements)
			{
				renderElement->Register(m_renderQueueRegistry);
				m_renderQueue.Insert(renderElement.GetElement());
			}

			m_renderQueueRegistry.Finalize();

			m_lastVisibilityHash = visibilityHash;
			m_rebuildElements = true;
		}

		// TODO: Don't sort every frame if no material pass requires distance sorting
		m_renderQueue.Sort([&](const RenderElement* element)
		{
			return element->ComputeSortingScore(frustum, m_renderQueueRegistry);
		});

		if (m_rebuildElements)
		{
			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				if (elementType >= m_elementRendererData.size() || !m_elementRendererData[elementType])
				{
					if (elementType >= m_elementRendererData.size())
						m_elementRendererData.resize(elementType + 1);

					m_elementRendererData[elementType] = elementRenderer.InstanciateData();
				}

				elementRenderer.Reset(*m_elementRendererData[elementType], renderFrame);
			});

			const auto& viewerInstance = m_viewer->GetViewerInstance();

			m_elementRegistry.ProcessRenderQueue(m_renderQueue, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
			{
				ElementRenderer& elementRenderer = m_elementRegistry.GetElementRenderer(elementType);

				m_renderStates.clear();
				m_renderStates.resize(elementCount);

				elementRenderer.Prepare(viewerInstance, *m_elementRendererData[elementType], renderFrame, elementCount, elements, m_renderStates.data());
			});

			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				elementRenderer.PrepareEnd(renderFrame, *m_elementRendererData[elementType]);
			});

			m_rebuildCommandBuffer = true;
			m_rebuildElements = false;
		}
	}

	void DepthPipelinePass::RegisterMaterialInstance(const MaterialInstance& materialInstance)
	{
		if (!materialInstance.HasPass(m_depthPassIndex))
			return;

		auto it = m_materialInstances.find(&materialInstance);
		if (it == m_materialInstances.end())
		{
			auto& matPassEntry = m_materialInstances[&materialInstance];
			matPassEntry.onMaterialInstancePipelineInvalidated.Connect(materialInstance.OnMaterialInstancePipelineInvalidated, [=](const MaterialInstance*, std::size_t passIndex)
			{
				if (passIndex != m_depthPassIndex)
					return;

				m_rebuildElements = true;
			});

			matPassEntry.onMaterialInstanceShaderBindingInvalidated.Connect(materialInstance.OnMaterialInstanceShaderBindingInvalidated, [=](const MaterialInstance*)
			{
				m_rebuildCommandBuffer = true;
			});
		}
		else
			it->second.usedCount++;
	}

	FramePass& DepthPipelinePass::RegisterToFrameGraph(FrameGraph& frameGraph, std::size_t depthBufferIndex)
	{
		FramePass& depthPrepass = frameGraph.AddPass("Depth pre-pass");
		depthPrepass.SetDepthStencilOutput(depthBufferIndex);
		depthPrepass.SetDepthStencilClear(1.f, 0);

		depthPrepass.SetExecutionCallback([&]()
		{
			return (m_rebuildCommandBuffer) ? FramePassExecution::UpdateAndExecute : FramePassExecution::Execute;
		});

		depthPrepass.SetCommandCallback([this](CommandBufferBuilder& builder, const FramePassEnvironment& /*env*/)
		{
			Recti viewport = m_viewer->GetViewport();

			builder.SetScissor(viewport);
			builder.SetViewport(viewport);

			const auto& viewerInstance = m_viewer->GetViewerInstance();

			m_elementRegistry.ProcessRenderQueue(m_renderQueue, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
			{
				ElementRenderer& elementRenderer = m_elementRegistry.GetElementRenderer(elementType);
				elementRenderer.Render(viewerInstance, *m_elementRendererData[elementType], builder, elementCount, elements);
			});

			m_rebuildCommandBuffer = false;
		});

		return depthPrepass;
	}

	void DepthPipelinePass::UnregisterMaterialInstance(const MaterialInstance& materialInstance)
	{
		auto it = m_materialInstances.find(&materialInstance);
		if (it != m_materialInstances.end())
		{
			if (--it->second.usedCount == 0)
				m_materialInstances.erase(it);
		}
	}
}
