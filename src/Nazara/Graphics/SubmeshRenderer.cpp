// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/SubmeshRenderer.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialProxy.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/RenderSubmesh.hpp>
#include <Nazara/Graphics/ShaderBindingCache.hpp>
#include <Nazara/Graphics/SkeletonInstance.hpp>
#include <Nazara/Graphics/TextureAsset.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderResources.hpp>

namespace Nz
{
	SubmeshRenderer::SubmeshRenderer(RenderDevice& device) :
	m_device(device)
	{
		m_pool = std::make_shared<PoolData>();
	}

	void SubmeshRenderer::ForEachIndirectBuffer(ElementRendererData& rendererData, FunctionRef<void(RenderBuffer& buffer, std::size_t commandCount)> callback)
	{
		auto& data = SafeCast<SubmeshRendererData&>(rendererData);

		std::size_t remainingElements = data.totalElementCount;
		for (const auto& buffer : data.drawIndirectBuffers)
		{
			std::size_t commandCount = std::min(remainingElements, IndirectCommandBufferCount);
			callback(*buffer, commandCount);
			remainingElements -= commandCount;
		}
	}

	RenderElementPool<RenderSubmesh>& SubmeshRenderer::GetPool()
	{
		return m_submeshPool;
	}

	std::unique_ptr<ElementRendererData> SubmeshRenderer::InstanciateData()
	{
		return std::make_unique<SubmeshRendererData>();
	}

	void SubmeshRenderer::Prepare(const RenderData& renderData, const SceneData& sceneData, const AbstractViewer& viewer, ElementRendererData& rendererData, RenderResources& renderResources, std::size_t elementCount, const Pointer<const RenderElement>* elements)
	{
		auto& data = SafeCast<SubmeshRendererData&>(rendererData);

		for (std::size_t i = 0; i < elementCount; ++i)
		{
			if (!data.currentIndirectBufferPtr)
			{
				std::shared_ptr<RenderBuffer> indirectBuffer;
				if (!m_pool->indirectBuffers.empty())
				{
					indirectBuffer = std::move(m_pool->indirectBuffers.back());
					m_pool->indirectBuffers.pop_back();
				}
				else
					indirectBuffer = m_device.InstantiateBuffer(IndirectCommandBufferCount * PredefinedIndirectDrawOffsets.totalSize, BufferUsage::IndirectBuffer | BufferUsage::MapSequentialWrite | BufferUsage::PersistentMapping | BufferUsage::StorageBuffer);

				data.indirectCommandIndex = 0;
				data.currentIndirectBufferPtr = static_cast<UInt8*>(indirectBuffer->Map(0, RenderBuffer::WholeSize));

				data.drawIndirectBuffers.push_back(std::move(indirectBuffer));
			}

			NazaraAssert(elements[i]->GetElementType() == UnderlyingCast(BasicRenderElement::Submesh));
			const RenderSubmesh& submesh = static_cast<const RenderSubmesh&>(*elements[i]);

			UInt8* indirectBuffer = data.currentIndirectBufferPtr + data.indirectCommandIndex * PredefinedIndirectDrawOffsets.totalSize;
			const Spheref& boundingSphere = submesh.GetBoundingSphere();
			static_assert(sizeof(Spheref) == 4 * sizeof(float));

			if (submesh.GetIndexBuffer() != nullptr)
			{
				DrawIndexedIndirectCommand drawIndirectCommand;
				drawIndirectCommand.firstIndex = 0;
				drawIndirectCommand.firstInstance = submesh.GetInstanceIndex();
				drawIndirectCommand.indexCount = submesh.GetIndexCount();
				drawIndirectCommand.instanceCount = 1;
				drawIndirectCommand.vertexOffset = 0;

				std::memcpy(indirectBuffer + PredefinedIndirectDrawOffsets.drawCommand, &drawIndirectCommand, sizeof(drawIndirectCommand));
			}
			else
			{
				DrawIndirectCommand drawIndirectCommand;
				drawIndirectCommand.firstVertex = 0;
				drawIndirectCommand.firstInstance = submesh.GetInstanceIndex();
				drawIndirectCommand.instanceCount = 1;
				drawIndirectCommand.vertexCount = submesh.GetIndexCount();

				std::memcpy(indirectBuffer + PredefinedIndirectDrawOffsets.drawCommand, &drawIndirectCommand, sizeof(drawIndirectCommand));
			}

			std::memcpy(indirectBuffer + PredefinedIndirectDrawOffsets.boundingSphere, &boundingSphere, sizeof(boundingSphere));

			data.indirectCommandIndex++;
			data.totalElementCount++;

			if (data.indirectCommandIndex >= IndirectCommandBufferCount)
				data.currentIndirectBufferPtr = nullptr;
		}
	}

	void SubmeshRenderer::Render(const RenderData& renderData, const SceneData& sceneData, const AbstractViewer& viewer, ElementRendererData& rendererData, RenderResources& renderResources, CommandBufferBuilder& commandBuffer, std::size_t elementCount, const Pointer<const RenderElement>* elements)
	{
		auto& data = SafeCast<SubmeshRendererData&>(rendererData);
		if (!data.references)
		{
			if (!m_pool->references.empty())
			{
				data.references = std::move(m_pool->references.back());
				m_pool->references.pop_back();
			}
			else
				data.references.emplace();
		}

		data.references->renderBuffers.insert(sceneData.directionalLightAtlasMapping);
		data.references->renderBuffers.insert(sceneData.directionalLights);
		data.references->renderBuffers.insert(sceneData.instanceBuffer);
		data.references->renderBuffers.insert(sceneData.pointLightAtlasMapping);
		data.references->renderBuffers.insert(sceneData.pointLights);
		data.references->renderBuffers.insert(sceneData.spotLightAtlasMapping);
		data.references->renderBuffers.insert(sceneData.spotLights);
		data.references->textures.insert(sceneData.shadowAtlas);

		const RenderBuffer* currentIndexBuffer = nullptr;
		const RenderBuffer* currentVertexBuffer = nullptr;
		const MaterialProxy* currentMaterialProxy = nullptr;
		const RenderPipeline* currentPipeline = nullptr;
		const ShaderBinding* currentInstanceShaderBinding = nullptr;
		const ShaderBinding* currentMaterialShaderBinding = nullptr;
		const ShaderBinding* currentViewerShaderBinding = nullptr;
		const ShaderBinding* currentSceneShaderBinding = nullptr;
		const SkeletonInstance* currentSkeletonInstance = nullptr;
		Recti currentScissorBox(-1, -1, -1, -1);

		std::size_t sceneSetHash = 0;
		std::size_t viewerSetHash = 0;

		for (std::size_t i = 0; i < elementCount; ++i)
		{
			NazaraAssert(elements[i]->GetElementType() == UnderlyingCast(BasicRenderElement::Submesh));
			const RenderSubmesh& submesh = static_cast<const RenderSubmesh&>(*elements[i]);

			if (const RenderPipeline* pipeline = submesh.GetRenderPipeline(); currentPipeline != pipeline)
			{
				commandBuffer.BindRenderPipeline(*pipeline);
				currentPipeline = pipeline;
			}

			if (const MaterialProxy* materialInstance = &submesh.GetMaterialProxy(); currentMaterialProxy != materialInstance)
			{
				currentMaterialProxy = materialInstance;
				currentMaterialProxy->FillRenderResourceReferences(*data.references);
				currentMaterialShaderBinding = nullptr;
			}

			if (const RenderBuffer* indexBuffer = submesh.GetIndexBuffer(); currentIndexBuffer != indexBuffer)
			{
				if (indexBuffer)
					commandBuffer.BindIndexBuffer(*indexBuffer, submesh.GetIndexType());

				currentIndexBuffer = indexBuffer;
			}

			if (const RenderBuffer* vertexBuffer = submesh.GetVertexBuffer(); currentVertexBuffer != vertexBuffer)
			{
				if (vertexBuffer)
					commandBuffer.BindVertexBuffer(0, *vertexBuffer);

				currentVertexBuffer = vertexBuffer;
			}

			if (const SkeletonInstance* skeletonInstance = submesh.GetSkeletonInstance(); currentSkeletonInstance != skeletonInstance)
			{
				currentInstanceShaderBinding = nullptr;
				currentSkeletonInstance = skeletonInstance;
			}

			const Recti& scissorBox = submesh.GetScissorBox();
			const Recti& targetScissorBox = (scissorBox.width >= 0) ? scissorBox : renderData.renderRegion;
			if (currentScissorBox != targetScissorBox)
			{
				commandBuffer.SetScissor(targetScissorBox);
				currentScissorBox = targetScissorBox;
			}

			if (!currentSceneShaderBinding || sceneSetHash != currentMaterialProxy->GetBindingSetHash(Material::SceneBindingSet))
			{
				sceneSetHash = currentMaterialProxy->GetBindingSetHash(Material::SceneBindingSet);

				ShaderBinding* sceneBinding = renderData.shaderBindingCache->GetSceneBinding(sceneSetHash, [&]
				{
					ShaderBindingPtr sceneBinding = currentPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(Material::SceneBindingSet);

					m_bindingCache.clear();
					currentMaterialProxy->FillSceneBindings(sceneData, m_bindingCache);
					sceneBinding->Update(m_bindingCache.data(), m_bindingCache.size());

					return sceneBinding;
				});

				commandBuffer.BindRenderShaderBinding(Material::SceneBindingSet, *sceneBinding);

				currentSceneShaderBinding = sceneBinding;
				currentViewerShaderBinding = nullptr;
			}

			if (!currentViewerShaderBinding || viewerSetHash != currentMaterialProxy->GetBindingSetHash(Material::ViewerBindingSet))
			{
				viewerSetHash = currentMaterialProxy->GetBindingSetHash(Material::ViewerBindingSet);

				ShaderBinding* viewerBinding = renderData.shaderBindingCache->GetViewerBinding(viewer.GetViewerInstance(), viewerSetHash, [&]
				{
					ShaderBindingPtr viewerBinding = currentPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(Material::ViewerBindingSet);

					m_bindingCache.clear();
					currentMaterialProxy->FillViewerBindings(viewer, m_bindingCache);
					viewerBinding->Update(m_bindingCache.data(), m_bindingCache.size());

					return viewerBinding;
				});

				commandBuffer.BindRenderShaderBinding(Material::ViewerBindingSet, *viewerBinding);

				currentMaterialShaderBinding = nullptr;
				currentViewerShaderBinding = viewerBinding;
			}

			if (!currentMaterialShaderBinding)
			{
				currentMaterialShaderBinding = &currentMaterialProxy->GetShaderBinding(renderResources);
				currentInstanceShaderBinding = nullptr;

				commandBuffer.BindRenderShaderBinding(Material::MaterialBindingSet, *currentMaterialShaderBinding);
			}

			if (!currentInstanceShaderBinding && currentMaterialProxy->GetBindingSetCount() > Material::InstanceBindingSet && currentSkeletonInstance)
			{
				ShaderBindingPtr instanceBinding = currentPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(Material::InstanceBindingSet);

				m_bindingCache.clear();
				currentMaterialProxy->FillSkeletonBindings(*currentSkeletonInstance, m_bindingCache);

				instanceBinding->Update(m_bindingCache.data(), m_bindingCache.size());

				commandBuffer.BindRenderShaderBinding(Material::InstanceBindingSet, *instanceBinding);

				currentInstanceShaderBinding = instanceBinding.get();

				data.shaderBindings.emplace_back(std::move(instanceBinding));
			}

			RenderBuffer* indirectBuffer = data.drawIndirectBuffers[data.drawIndirectBufferIndex].get();

			if (currentIndexBuffer)
				commandBuffer.DrawIndexedIndirect(*indirectBuffer, data.drawElementCounter * PredefinedIndirectDrawOffsets.totalSize, 1, PredefinedIndirectDrawOffsets.totalSize);
			else
				commandBuffer.DrawIndirect(*indirectBuffer, data.drawElementCounter * PredefinedIndirectDrawOffsets.totalSize, 1, PredefinedIndirectDrawOffsets.totalSize);

			data.drawElementCounter++;
			if (data.drawElementCounter >= IndirectCommandBufferCount)
			{
				data.drawElementCounter = 0;
				data.drawIndirectBufferIndex++;
			}
		}
	}

	void SubmeshRenderer::Reset(ElementRendererData& rendererData, RenderResources& renderResources)
	{
		auto& data = SafeCast<SubmeshRendererData&>(rendererData);

		for (auto& indirectBuffer : data.drawIndirectBuffers)
		{
			renderResources.PushReleaseCallback([pool = m_pool, buffer = std::move(indirectBuffer)]() mutable
			{
				pool->indirectBuffers.push_back(std::move(buffer));
			});
		}
		data.drawIndirectBuffers.clear();

		data.currentIndirectBufferPtr = nullptr;
		data.drawElementCounter = 0;
		data.drawIndirectBufferIndex = 0;
		data.totalElementCount = 0;

		if (data.references)
		{
			renderResources.PushReleaseCallback([pool = m_pool, references = std::move(*data.references)]() mutable
			{
				references.Clear();
				pool->references.push_back(std::move(references));
			});
			data.references.reset();
		}

		for (auto& shaderBinding : data.shaderBindings)
			renderResources.PushForRelease(std::move(shaderBinding));
		data.shaderBindings.clear();
	}
}
