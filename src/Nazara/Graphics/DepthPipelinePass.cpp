// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DepthPipelinePass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/ElementRendererRegistry.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	void DepthPipelinePass::Prepare(FrameData& frameData)
	{
		if (m_lastVisibilityHash != frameData.visibilityHash || m_rebuildElements) //< FIXME
		{
			frameData.renderResources.PushForRelease(std::move(m_renderElements));
			m_renderElements.clear();

			for (const auto& renderableData : frameData.visibleRenderables)
			{
				InstancedRenderable::ElementData elementData{
					&renderableData.scissorBox,
					renderableData.skeletonInstance,
					renderableData.worldInstance
				};

				renderableData.instancedRenderable->BuildElement(m_elementRegistry, elementData, m_passIndex, m_renderElements);
			}

			m_renderQueueRegistry.Clear();
			m_renderQueue.Clear();

			for (const auto& renderElement : m_renderElements)
			{
				renderElement->Register(m_renderQueueRegistry);
				m_renderQueue.Insert(renderElement.GetElement());
			}

			m_renderQueueRegistry.Finalize();

			m_lastVisibilityHash = frameData.visibilityHash;
			m_rebuildElements = true;
		}

		// TODO: Don't sort every frame if no material pass requires distance sorting
		m_renderQueue.Sort([&](const RenderElement* element)
		{
			return element->ComputeSortingScore(frameData.frustum, m_renderQueueRegistry);
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

				elementRenderer.Reset(*m_elementRendererData[elementType], frameData.renderResources);
			});

			const auto& viewerInstance = m_viewer->GetViewerInstance();

			ElementRenderer::RenderStates defaultRenderStates{};

			m_elementRegistry.ProcessRenderQueue(m_renderQueue, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
			{
				ElementRenderer& elementRenderer = m_elementRegistry.GetElementRenderer(elementType);

				elementRenderer.Prepare(viewerInstance, *m_elementRendererData[elementType], frameData.renderResources, elementCount, elements, SparsePtr(&defaultRenderStates, 0));
			});

			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				elementRenderer.PrepareEnd(frameData.renderResources, *m_elementRendererData[elementType]);
			});

			m_rebuildCommandBuffer = true;
			m_rebuildElements = false;
		}
	}

	void DepthPipelinePass::RegisterMaterialInstance(const MaterialInstance& materialInstance)
	{
		if (!materialInstance.HasPass(m_passIndex))
			return;

		auto it = m_materialInstances.find(&materialInstance);
		if (it == m_materialInstances.end())
		{
			auto& matPassEntry = m_materialInstances[&materialInstance];
			matPassEntry.onMaterialInstancePipelineInvalidated.Connect(materialInstance.OnMaterialInstancePipelineInvalidated, [this](const MaterialInstance*, std::size_t passIndex)
			{
				if (passIndex != m_passIndex)
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

	FramePass& DepthPipelinePass::RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs)
	{
		if (inputOuputs.inputAttachments.size() > 0)
			throw std::runtime_error("no input expected");

		if (inputOuputs.outputAttachments.size() > 0)
			throw std::runtime_error("no output expected");

		if (inputOuputs.depthStencilInput != InvalidAttachmentIndex)
			throw std::runtime_error("no depth-stencil input expected");

		if (inputOuputs.depthStencilOutput == InvalidAttachmentIndex)
			throw std::runtime_error("expected depth-stencil output");

		FramePass& depthPrepass = frameGraph.AddPass(m_passName);
		depthPrepass.SetDepthStencilOutput(inputOuputs.depthStencilOutput);
		depthPrepass.SetDepthStencilClear(1.f, 0);

		depthPrepass.SetExecutionCallback([&]
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

	std::size_t DepthPipelinePass::GetMaterialPassIndex(const ParameterList& parameters)
	{
		Result<long long, ParameterList::Error> passIndexResult = parameters.GetIntegerParameter("MatPassIndex");
		if (passIndexResult.IsOk())
			return passIndexResult.GetValue();
		// TODO: Log error if key is present but not of the right

		Result<std::string_view, ParameterList::Error> passResult = parameters.GetStringViewParameter("MatPass");
		if (passResult.IsOk())
		{
			auto& materialPassRegistry = Graphics::Instance()->GetMaterialPassRegistry();

			std::string_view passName = passResult.GetValue();
			return materialPassRegistry.GetPassIndex(passName);
		}
		// TODO: Log error if key is present but not of the right

		throw std::runtime_error("DepthPipelinePass expect either MatPass or MatPassIndex parameter");
	}
}
