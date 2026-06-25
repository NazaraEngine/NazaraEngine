// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/ForwardPipelinePass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/DirectionalLight.hpp>
#include <Nazara/Graphics/DirectionalLightShadowData.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/ElementRendererRegistry.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/PointLight.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/ShaderTransfer.hpp>
#include <Nazara/Graphics/SpotLight.hpp>
#include <Nazara/Graphics/SpotLightShadowData.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>

namespace Nz
{
	ForwardPipelinePass::ForwardPipelinePass(PassData& passData, std::string passName, const ParameterList& parameters) :
	FramePipelinePass(FramePipelineNotification::ElementInvalidation | FramePipelineNotification::MaterialInstanceRegistration),
	m_lastVisibilityHash(0),
	m_passName(std::move(passName)),
	m_viewer(passData.viewer),
	m_elementRegistry(passData.elementRegistry),
	m_pipeline(passData.pipeline),
	m_pendingLightUploadAllocation(nullptr),
	m_renderMask(MaxValue()),
	m_rebuildElements(false)
	{
		if (auto result = parameters.GetIntegerParameter("RenderMask", false); result && result.GetValue() >= 0 && result.GetValue() < MaxValue<UInt32>())
			m_renderMask = SafeCaster(result.GetValue());
		else if (result.GetError() != Nz::ParameterList::Error::MissingValue)
			throw std::runtime_error("parameter RenderMask has incorrect value");

		Graphics* graphics = Graphics::Instance();
		m_forwardPassIndex = graphics->GetMaterialPassRegistry().GetPassIndex("ForwardPass");
	}

	void ForwardPipelinePass::Prepare(FrameData& frameData)
	{
		if (m_lastVisibilityHash != frameData.visibilityHash || m_rebuildElements) //< FIXME
		{
			frameData.renderResources.PushForRelease(std::move(m_renderElements));
			m_renderElements.clear();

			for (const auto& renderableData : frameData.visibleRenderables)
			{
				if ((m_renderMask & renderableData.renderMask) == 0)
					continue;

				InstancedRenderable::ElementData elementData{
					&renderableData.scissorBox,
					renderableData.skeletonInstance,
					renderableData.worldInstance
				};

				renderableData.instancedRenderable->BuildElement(m_elementRegistry, elementData, m_forwardPassIndex, m_renderElements);
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
			InvalidateElements();
		}

		// TODO: Don't sort every frame if no material pass requires distance sorting
		m_renderQueue.Sort([&](const RenderElement* element)
		{
			return element->ComputeSortingScore(frameData.frustum, m_renderQueueRegistry);
		});
	}

	void ForwardPipelinePass::RegisterMaterialInstance(const MaterialInstance& materialInstance)
	{
		if (!materialInstance.HasPass(m_forwardPassIndex))
			return;

		auto it = m_materialInstances.find(&materialInstance);
		if (it == m_materialInstances.end())
		{
			auto& matPassEntry = m_materialInstances[&materialInstance];
			matPassEntry.onMaterialInstancePipelineInvalidated.Connect(materialInstance.OnMaterialInstancePipelineInvalidated, [this](const MaterialInstance*, std::size_t passIndex)
			{
				if (passIndex != m_forwardPassIndex)
					return;

				m_rebuildElements = true;
			});

			matPassEntry.onMaterialInstanceShaderBindingInvalidated.Connect(materialInstance.OnMaterialInstanceShaderBindingInvalidated, [this](const MaterialInstance*)
			{
				m_rebuildElements = true;
			});
		}
		else
			it->second.usedCount++;
	}

	FramePass& ForwardPipelinePass::RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs)
	{
		FramePass& forwardPass = frameGraph.AddPass(m_passName);
		for (auto&& inputData : inputOuputs.inputAttachments)
			forwardPass.AddInput(inputData.attachmentIndex);

		for (auto&& outputData : inputOuputs.outputAttachments)
		{
			std::size_t outputIndex = forwardPass.AddOutput(outputData.attachmentIndex);

			std::visit(Overloaded{
				[](DontClear) {},
				[&](const Color& color)
				{
					forwardPass.SetClearColor(outputIndex, color);
				},
				[&](ViewerClearValue)
				{
					forwardPass.SetClearColor(outputIndex, m_viewer->GetClearColor());
				}
			}, outputData.clearColor);
		}

		if (inputOuputs.depthStencilInput != FramePipelinePass::InvalidAttachmentIndex)
			forwardPass.SetDepthStencilInput(inputOuputs.depthStencilInput);
		else if (inputOuputs.depthStencilOutput != InvalidAttachmentIndex)
		{
			std::visit(Overloaded{
				[](DontClear) {},
				[&](float depth)
				{
					forwardPass.SetDepthStencilClear(depth, 0);
				},
				[&](ViewerClearValue)
				{
					forwardPass.SetDepthStencilClear(m_viewer->GetClearDepth(), 0);
				}
			}, inputOuputs.clearDepth);
		}

		if (inputOuputs.depthStencilOutput != InvalidAttachmentIndex)
			forwardPass.SetDepthStencilOutput(inputOuputs.depthStencilOutput);

		forwardPass.SetExecutionCallback([&]()
		{
			return FramePassExecution::UpdateAndExecute;
		});

		forwardPass.SetCommandCallback([this](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				if (elementType >= m_elementRendererData.size())
					m_elementRendererData.resize(elementType + 1);

				if (!m_elementRendererData[elementType])
					m_elementRendererData[elementType] = elementRenderer.InstanciateData();
			});

			ElementRenderer::RenderData renderData;
			renderData.directionalLights = RenderBufferView(m_pipeline.GetDirectionalLightBuffer().get());
			renderData.pointLights = RenderBufferView(m_pipeline.GetPointLightBuffer().get());
			renderData.spotLights = RenderBufferView(m_pipeline.GetSpotLightBuffer().get());

			m_elementRegistry.ProcessRenderQueue(m_renderQueue, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
			{
				ElementRenderer& elementRenderer = m_elementRegistry.GetElementRenderer(elementType);
				elementRenderer.Prepare(renderData, *m_viewer, *m_elementRendererData[elementType], env.renderResources, elementCount, elements);
			});

			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				elementRenderer.PrepareEnd(*m_elementRendererData[elementType], env.renderResources, builder);
			});
		});

		forwardPass.SetRenderCallback([this](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			Recti viewport = m_viewer->GetViewport();

			builder.SetScissor(viewport);
			builder.SetViewport(viewport);

			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				if (elementType >= m_elementRendererData.size())
					m_elementRendererData.resize(elementType + 1);

				if (!m_elementRendererData[elementType])
					m_elementRendererData[elementType] = elementRenderer.InstanciateData();
			});

			ElementRenderer::RenderData renderData;
			renderData.directionalLights = RenderBufferView(m_pipeline.GetDirectionalLightBuffer().get());
			renderData.directionalLightAtlasMapping = RenderBufferView(m_pipeline.GetDirectionalShadowMappingBuffer().get());
			renderData.pointLights = RenderBufferView(m_pipeline.GetPointLightBuffer().get());
			renderData.pointLightAtlasMapping = RenderBufferView(m_pipeline.GetPointShadowMappingBuffer().get());
			renderData.renderRegion = viewport;
			renderData.shadowAtlas = m_pipeline.GetShadowAtlasTexture().get();
			renderData.spotLights = RenderBufferView(m_pipeline.GetSpotLightBuffer().get());
			renderData.spotLightAtlasMapping = RenderBufferView(m_pipeline.GetSpotShadowMappingBuffer().get());

			m_elementRegistry.ProcessRenderQueue(m_renderQueue, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
			{
				ElementRenderer& elementRenderer = m_elementRegistry.GetElementRenderer(elementType);
				elementRenderer.Render(renderData, *m_viewer, *m_elementRendererData[elementType], env.renderResources, builder, elementCount, elements);
			});

			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				elementRenderer.Reset(*m_elementRendererData[elementType], env.renderResources);
			});
		});

		return forwardPass;
	}

	void ForwardPipelinePass::UnregisterMaterialInstance(const MaterialInstance& materialInstance)
	{
		auto it = m_materialInstances.find(&materialInstance);
		if (it != m_materialInstances.end())
		{
			if (--it->second.usedCount == 0)
				m_materialInstances.erase(it);
		}
	}

	void ForwardPipelinePass::OnTransfer(RenderResources& /*renderFrame*/, CommandBufferBuilder& builder)
	{
		assert(m_pendingLightUploadAllocation);
		builder.CopyBuffer(*m_pendingLightUploadAllocation, RenderBufferView(m_lightDataBuffer.get()));
		m_pendingLightUploadAllocation = nullptr;
	}
}
