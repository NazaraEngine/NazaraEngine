// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardPipelinePass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/ElementRendererRegistry.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	ForwardPipelinePass::ForwardPipelinePass(FramePipeline& owner, ElementRendererRegistry& elementRegistry, AbstractViewer* viewer) :
	m_lastVisibilityHash(0),
	m_viewer(viewer),
	m_elementRegistry(elementRegistry),
	m_pipeline(owner),
	m_rebuildCommandBuffer(false),
	m_rebuildElements(false)
	{
		Graphics* graphics = Graphics::Instance();
		m_forwardPassIndex = graphics->GetMaterialPassRegistry().GetPassIndex("ForwardPass");
		m_lightUboPool = std::make_shared<LightUboPool>();
	}

	void ForwardPipelinePass::Prepare(RenderFrame& renderFrame, const Frustumf& frustum, const std::vector<FramePipelinePass::VisibleRenderable>& visibleRenderables, const std::vector<const Light*>& visibleLights, std::size_t visibilityHash)
	{
		if (m_lastVisibilityHash != visibilityHash || m_rebuildElements) //< FIXME
		{
			renderFrame.PushForRelease(std::move(m_renderElements));
			m_renderElements.clear();
			m_renderQueueRegistry.Clear();
			m_renderQueue.Clear();
			m_lightBufferPerLights.clear();
			m_lightPerRenderElement.clear();

			for (auto& lightDataUbo : m_lightDataBuffers)
			{
				renderFrame.PushReleaseCallback([pool = m_lightUboPool, lightUbo = std::move(lightDataUbo.renderBuffer)]() mutable
				{
					pool->lightUboBuffers.push_back(std::move(lightUbo));
				});
			}
			m_lightDataBuffers.clear();

			Graphics* graphics = Graphics::Instance();

			PredefinedLightData lightOffsets = PredefinedLightData::GetOffsets();
			std::size_t lightUboAlignedSize = AlignPow2(lightOffsets.totalSize, SafeCast<std::size_t>(graphics->GetRenderDevice()->GetDeviceInfo().limits.minUniformBufferOffsetAlignment));

			UploadPool& uploadPool = renderFrame.GetUploadPool();

			for (const auto& renderableData : visibleRenderables)
			{
				BoundingVolumef renderableBoundingVolume(renderableData.instancedRenderable->GetAABB());
				renderableBoundingVolume.Update(renderableData.worldInstance->GetWorldMatrix());

				// Select lights
				m_renderableLights.clear();
				for (const Light* light : visibleLights)
				{
					const BoundingVolumef& boundingVolume = light->GetBoundingVolume();
					if (boundingVolume.Intersect(renderableBoundingVolume.aabb))
						m_renderableLights.push_back(light);
				}

				// Sort lights
				std::sort(m_renderableLights.begin(), m_renderableLights.end(), [&](const Light* lhs, const Light* rhs)
				{
					return lhs->ComputeContributionScore(renderableBoundingVolume) < rhs->ComputeContributionScore(renderableBoundingVolume);
				});

				std::size_t lightCount = std::min(m_renderableLights.size(), MaxLightCountPerDraw);

				LightKey lightKey;
				lightKey.fill(nullptr);
				for (std::size_t i = 0; i < lightCount; ++i)
					lightKey[i] = m_renderableLights[i];

				RenderBufferView lightUboView;

				auto it = m_lightBufferPerLights.find(lightKey);
				if (it == m_lightBufferPerLights.end())
				{
					// Prepare light ubo upload

					// Find light ubo
					LightDataUbo* targetLightData = nullptr;
					for (auto& lightUboData : m_lightDataBuffers)
					{
						if (lightUboData.offset + lightUboAlignedSize <= lightUboData.renderBuffer->GetSize())
						{
							targetLightData = &lightUboData;
							break;
						}
					}

					if (!targetLightData)
					{
						// Make a new light UBO
						auto& lightUboData = m_lightDataBuffers.emplace_back();

						// Reuse from pool if possible
						if (!m_lightUboPool->lightUboBuffers.empty())
						{
							lightUboData.renderBuffer = m_lightUboPool->lightUboBuffers.back();
							m_lightUboPool->lightUboBuffers.pop_back();
						}
						else
							lightUboData.renderBuffer = graphics->GetRenderDevice()->InstantiateBuffer(BufferType::Uniform, 256 * lightUboAlignedSize, BufferUsage::DeviceLocal | BufferUsage::Dynamic | BufferUsage::Write);

						targetLightData = &lightUboData;
					}

					assert(targetLightData);
					if (!targetLightData->allocation)
						targetLightData->allocation = &uploadPool.Allocate(targetLightData->renderBuffer->GetSize());

					void* lightDataPtr = static_cast<UInt8*>(targetLightData->allocation->mappedPtr) + targetLightData->offset;
					AccessByOffset<UInt32&>(lightDataPtr, lightOffsets.lightCountOffset) = SafeCast<UInt32>(lightCount);

					UInt8* lightPtr = static_cast<UInt8*>(lightDataPtr) + lightOffsets.lightsOffset;
					for (std::size_t i = 0; i < lightCount; ++i)
					{
						m_renderableLights[i]->FillLightData(lightPtr);
						lightPtr += lightOffsets.lightSize;
					}

					// Associate render element with light ubo
					lightUboView = RenderBufferView(targetLightData->renderBuffer.get(), targetLightData->offset, lightUboAlignedSize);

					targetLightData->offset += lightUboAlignedSize;

					m_lightBufferPerLights.emplace(lightKey, lightUboView);
				}
				else
					lightUboView = it->second;

				InstancedRenderable::ElementData elementData{
					&renderableData.scissorBox,
					renderableData.skeletonInstance,
					renderableData.worldInstance
				};

				std::size_t previousCount = m_renderElements.size();
				renderableData.instancedRenderable->BuildElement(m_elementRegistry, elementData, m_forwardPassIndex, m_renderElements);
				for (std::size_t i = previousCount; i < m_renderElements.size(); ++i)
				{
					const RenderElement* element = m_renderElements[i].GetElement();
					m_lightPerRenderElement.emplace(element, lightUboView);
				}
			}

			for (const auto& renderElement : m_renderElements)
			{
				renderElement->Register(m_renderQueueRegistry);
				m_renderQueue.Insert(renderElement.GetElement());
			}

			m_renderQueueRegistry.Finalize();

			renderFrame.Execute([&](CommandBufferBuilder& builder)
			{
				builder.BeginDebugRegion("Light UBO Update", Color::Yellow);
				{
					for (auto& lightUboData : m_lightDataBuffers)
					{
						if (!lightUboData.allocation)
							continue;

						builder.CopyBuffer(*lightUboData.allocation, RenderBufferView(lightUboData.renderBuffer.get(), 0, lightUboData.offset));
					}

					builder.PostTransferBarrier();
				}
				builder.EndDebugRegion();
			}, QueueType::Transfer);

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
				if (elementType >= m_elementRendererData.size())
					m_elementRendererData.resize(elementType + 1);

				if (!m_elementRendererData[elementType])
					m_elementRendererData[elementType] = elementRenderer.InstanciateData();

				elementRenderer.Reset(*m_elementRendererData[elementType], renderFrame);
			});

			const auto& viewerInstance = m_viewer->GetViewerInstance();

			auto& lightPerRenderElement = m_lightPerRenderElement;
			m_elementRegistry.ProcessRenderQueue(m_renderQueue, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
			{
				ElementRenderer& elementRenderer = m_elementRegistry.GetElementRenderer(elementType);

				m_renderStates.clear();

				m_renderStates.reserve(elementCount);
				for (std::size_t i = 0; i < elementCount; ++i)
				{
					auto it = lightPerRenderElement.find(elements[i]);
					assert(it != lightPerRenderElement.end());

					auto& renderStates = m_renderStates.emplace_back();
					renderStates.lightData = it->second;
				}

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

	void ForwardPipelinePass::RegisterMaterialInstance(const MaterialInstance& materialInstance)
	{
		if (!materialInstance.HasPass(m_forwardPassIndex))
			return;

		auto it = m_materialInstances.find(&materialInstance);
		if (it == m_materialInstances.end())
		{
			auto& matPassEntry = m_materialInstances[&materialInstance];
			matPassEntry.onMaterialInstancePipelineInvalidated.Connect(materialInstance.OnMaterialInstancePipelineInvalidated, [=](const MaterialInstance*, std::size_t passIndex)
			{
				if (passIndex != m_forwardPassIndex)
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

	FramePass& ForwardPipelinePass::RegisterToFrameGraph(FrameGraph& frameGraph, std::size_t colorBufferIndex, std::size_t depthBufferIndex, bool hasDepthPrepass)
	{
		FramePass& forwardPass = frameGraph.AddPass("Forward pass");
		forwardPass.AddOutput(colorBufferIndex);
		if (hasDepthPrepass)
			forwardPass.SetDepthStencilInput(depthBufferIndex);
		else
			forwardPass.SetDepthStencilOutput(depthBufferIndex);

		forwardPass.SetClearColor(0, m_viewer->GetClearColor());
		forwardPass.SetDepthStencilClear(1.f, 0);

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
}
