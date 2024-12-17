// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/ForwardPipelinePass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/DirectionalLight.hpp>
#include <Nazara/Graphics/DirectionalLightShadowData.hpp>
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
	m_handleLights(true),
	m_rebuildCommandBuffer(false),
	m_rebuildElements(false)
	{
		if (auto result = parameters.GetBooleanParameter("Lighting", false); result)
			m_handleLights = result.GetValue();
		else if (result.GetError() != Nz::ParameterList::Error::MissingValue)
			throw std::runtime_error("parameter Lighting has incorrect value");

		if (auto result = parameters.GetIntegerParameter("RenderMask", false); result && result.GetValue() >= 0 && result.GetValue() < Nz::MaxValue<UInt32>())
			m_renderMask = result.GetValue();
		else if (result.GetError() != Nz::ParameterList::Error::MissingValue)
			throw std::runtime_error("parameter RenderMask has incorrect value");

		Graphics* graphics = Graphics::Instance();
		m_forwardPassIndex = graphics->GetMaterialPassRegistry().GetPassIndex("ForwardPass");

		if (m_handleLights)
		{
			std::size_t lightUboAlignedSize = AlignPow2(PredefinedLightOffsets.totalSize, SafeCast<std::size_t>(graphics->GetRenderDevice()->GetDeviceInfo().limits.minUniformBufferOffsetAlignment));
			m_lightDataBuffer = graphics->GetRenderDevice()->InstantiateBuffer(BufferType::Uniform, lightUboAlignedSize, BufferUsage::DeviceLocal | BufferUsage::Dynamic | BufferUsage::Write);
			m_lightDataBuffer->UpdateDebugName("Lights buffer");

			m_renderState.lightData = RenderBufferView(m_lightDataBuffer.get());
		}
	}

	void ForwardPipelinePass::Prepare(FrameData& frameData)
	{
		NazaraAssertMsg(!m_handleLights || frameData.visibleLights, "visible lights must be valid");

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

		if (m_handleLights)
			PrepareLights(frameData.renderResources, frameData.frustum, *frameData.visibleLights);

		if (m_rebuildElements)
		{
			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				if (elementType >= m_elementRendererData.size())
					m_elementRendererData.resize(elementType + 1);

				if (!m_elementRendererData[elementType])
					m_elementRendererData[elementType] = elementRenderer.InstanciateData();

				elementRenderer.Reset(*m_elementRendererData[elementType], frameData.renderResources);
			});

			const ViewerInstance& viewerInstance = m_viewer->GetViewerInstance();

			m_elementRegistry.ProcessRenderQueue(m_renderQueue, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
			{
				ElementRenderer& elementRenderer = m_elementRegistry.GetElementRenderer(elementType);
				elementRenderer.Prepare(viewerInstance, *m_elementRendererData[elementType], frameData.renderResources, elementCount, elements, SparsePtr(&m_renderState, 0));
			});

			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				elementRenderer.PrepareEnd(frameData.renderResources, *m_elementRendererData[elementType]);
			});

			m_rebuildCommandBuffer = true;
			m_rebuildElements = false;
		}
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
		if (inputOuputs.inputAttachments.size() > 0)
			throw std::runtime_error("no input expected");

		if (inputOuputs.outputAttachments.size() != 1)
			throw std::runtime_error("one output expected");

		FramePass& forwardPass = frameGraph.AddPass(m_passName);

		for (auto&& outputData : inputOuputs.outputAttachments)
		{
			std::size_t outputIndex = forwardPass.AddOutput(outputData.attachmentIndex);

			std::visit(Nz::Overloaded{
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
			std::visit(Nz::Overloaded{
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
			return (m_rebuildCommandBuffer) ? FramePassExecution::UpdateAndExecute : FramePassExecution::Execute;
		});

		forwardPass.SetCommandCallback([this](CommandBufferBuilder& builder, const FramePassEnvironment& /*env*/)
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

	void ForwardPipelinePass::PrepareDirectionalLights(void* lightMemory)
	{
		std::size_t lightCount = std::min(m_directionalLights.size(), PredefinedLightData::MaxLightCount);

		AccessByOffset<UInt32&>(lightMemory, PredefinedLightOffsets.directionalLightCountOffset) = SafeCast<UInt32>(lightCount);
		for (std::size_t i = 0; i < lightCount; ++i)
		{
			UInt8* basePtr = static_cast<UInt8*>(lightMemory) + PredefinedLightOffsets.directionalLightsOffset + PredefinedDirectionalLightOffsets.totalSize * i;

			const DirectionalLight* light = m_directionalLights[i].light;
			ShaderTransfer::WriteLight(light, basePtr);

			// Shadowmap handling
			const Texture* shadowmap = m_pipeline.RetrieveLightShadowmap(m_directionalLights[i].lightIndex, m_viewer);
			if (shadowmap)
			{
				const DirectionalLightShadowData* shadowData = SafeCast<const DirectionalLightShadowData*>(m_pipeline.RetrieveLightShadowData(m_directionalLights[i].lightIndex));
				ShaderTransfer::WriteLightShadowData(m_viewer, shadowData, basePtr);
			}

			if (m_renderState.shadowMapsDirectional[i] != shadowmap)
			{
				m_renderState.shadowMapsDirectional[i] = shadowmap;
				InvalidateElements();
			}
		}
	}

	void ForwardPipelinePass::PreparePointLights(void* lightMemory)
	{
		std::size_t lightCount = std::min(m_pointLights.size(), PredefinedLightData::MaxLightCount);

		AccessByOffset<UInt32&>(lightMemory, PredefinedLightOffsets.pointLightCountOffset) = SafeCast<UInt32>(lightCount);
		for (std::size_t i = 0; i < lightCount; ++i)
		{
			UInt8* basePtr = static_cast<UInt8*>(lightMemory) + PredefinedLightOffsets.pointLightsOffset + PredefinedPointLightOffsets.totalSize * i;

			const PointLight* light = m_pointLights[i].light;
			ShaderTransfer::WriteLight(light, basePtr);

			// Shadowmap handling
			const Texture* shadowmap = m_pipeline.RetrieveLightShadowmap(m_pointLights[i].lightIndex, m_viewer);
			if (m_renderState.shadowMapsPoint[i] != shadowmap)
			{
				m_renderState.shadowMapsPoint[i] = shadowmap;
				InvalidateElements();
			}
		}
	}

	void ForwardPipelinePass::PrepareSpotLights(void* lightMemory)
	{
		std::size_t lightCount = std::min(m_spotLights.size(), PredefinedLightData::MaxLightCount);

		AccessByOffset<UInt32&>(lightMemory, PredefinedLightOffsets.spotLightCountOffset) = SafeCast<UInt32>(lightCount);
		for (std::size_t i = 0; i < lightCount; ++i)
		{
			UInt8* basePtr = static_cast<UInt8*>(lightMemory) + PredefinedLightOffsets.spotLightsOffset + PredefinedSpotLightOffsets.totalSize * i;

			const SpotLight* light = m_spotLights[i].light;

			const Color& lightColor = light->GetColor();
			ShaderTransfer::WriteLight(light, basePtr);

			// Shadowmap handling
			const Texture* shadowmap = m_pipeline.RetrieveLightShadowmap(m_spotLights[i].lightIndex, m_viewer);
			if (m_renderState.shadowMapsSpot[i] != shadowmap)
			{
				m_renderState.shadowMapsSpot[i] = shadowmap;
				InvalidateElements();
			}
		}
	}

	void ForwardPipelinePass::PrepareLights(RenderResources& renderResources, const Frustumf& frustum, const Bitset<UInt64>& visibleLights)
	{
		// Select lights
		m_directionalLights.clear();
		m_pointLights.clear();
		m_spotLights.clear();
		for (std::size_t lightIndex : visibleLights.IterBits())
		{
			const Light* light = m_pipeline.RetrieveLight(lightIndex);

			switch (light->GetLightType())
			{
				case UnderlyingCast(BasicLightType::Directional):
					m_directionalLights.push_back({ SafeCast<const DirectionalLight*>(light), lightIndex, 0.f });
					break;

				case UnderlyingCast(BasicLightType::Point):
					m_pointLights.push_back({ SafeCast<const PointLight*>(light), lightIndex, light->ComputeContributionScore(frustum) });
					break;

				case UnderlyingCast(BasicLightType::Spot):
					m_spotLights.push_back({ SafeCast<const SpotLight*>(light), lightIndex, light->ComputeContributionScore(frustum) });
					break;
			}
		}

		// Sort lights
		std::sort(m_pointLights.begin(), m_pointLights.end(), [&](const RenderableLight<PointLight>& lhs, const RenderableLight<PointLight>& rhs)
		{
			return lhs.contributionScore < rhs.contributionScore;
		});

		std::sort(m_spotLights.begin(), m_spotLights.end(), [&](const RenderableLight<SpotLight>& lhs, const RenderableLight<SpotLight>& rhs)
		{
			return lhs.contributionScore < rhs.contributionScore;
		});

		UploadPool& uploadPool = renderResources.GetUploadPool();

		auto& lightAllocation = uploadPool.Allocate(m_lightDataBuffer->GetSize());
		PrepareDirectionalLights(lightAllocation.mappedPtr);
		PreparePointLights(lightAllocation.mappedPtr);
		PrepareSpotLights(lightAllocation.mappedPtr);

		m_pendingLightUploadAllocation = &lightAllocation;
		m_pipeline.QueueTransfer(this);
	}
}
