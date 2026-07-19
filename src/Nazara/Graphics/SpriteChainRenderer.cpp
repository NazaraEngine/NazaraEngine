// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/SpriteChainRenderer.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialProxy.hpp>
#include <Nazara/Graphics/RenderSpriteChain.hpp>
#include <Nazara/Graphics/ShaderBindingCache.hpp>
#include <Nazara/Graphics/TextureAsset.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Renderer/GpuCommandBufferBuilder.hpp>
#include <Nazara/Renderer/GpuResources.hpp>
#include <Nazara/Renderer/GpuUploadPool.hpp>
#include <utility>

namespace Nz
{
	SpriteChainRenderer::SpriteChainRenderer(GpuDevice& device) :
	m_device(device)
	{
		m_pool = std::make_shared<PoolData>();

		std::size_t maxQuadCount = RenderSpriteChain::MaxSpritePerChain;
		std::size_t indexCount = 6 * maxQuadCount;

		// Generate indices for quad (0, 1, 2, 2, 1, 3, ...)
		std::vector<UInt16> indices(indexCount);
		UInt16* indexPtr = indices.data();

		for (std::size_t i = 0; i < maxQuadCount; ++i)
		{
			UInt16 index = static_cast<UInt16>(i);

			*indexPtr++ = index * 4 + 0;
			*indexPtr++ = index * 4 + 1;
			*indexPtr++ = index * 4 + 2;

			*indexPtr++ = index * 4 + 2;
			*indexPtr++ = index * 4 + 1;
			*indexPtr++ = index * 4 + 3;
		}

		m_indexBuffer = m_device.InstantiateBuffer(indexCount * sizeof(UInt16), BufferUsage::IndexBuffer | BufferUsage::DeviceLocal, indices.data());
	}

	void SpriteChainRenderer::ForEachIndirectBuffer(ElementRendererData& rendererData, FunctionRef<void(GpuBuffer& buffer, std::size_t commandCount)> callback)
	{
		// TODO
	}

	RenderElementPool<RenderSpriteChain>& SpriteChainRenderer::GetPool()
	{
		return m_spriteChainPool;
	}

	std::unique_ptr<ElementRendererData> SpriteChainRenderer::InstanciateData()
	{
		return std::make_unique<SpriteChainRendererData>();
	}

	void SpriteChainRenderer::Prepare(const RenderData& renderData, const SceneData& sceneData, const AbstractViewer& viewer, ElementRendererData& rendererData, GpuResources& renderResources, std::size_t elementCount, const Pointer<const RenderElement>* elements)
	{
		auto& data = static_cast<SpriteChainRendererData&>(rendererData);

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

		Recti invalidScissorBox(-1, -1, -1, -1);

		std::size_t oldDrawCallCount = data.drawCalls.size();

		constexpr Nz::UInt64 VertexBufferSize = 4 * RenderSpriteChain::MaxSpritePerChain * sizeof(Nz::VertexStruct_XYZ_Color_UV); //< Most common vertex struct but not a restriction

		for (std::size_t i = 0; i < elementCount; ++i)
		{
			assert(elements[i]->GetElementType() == UnderlyingCast(BasicRenderElement::SpriteChain));
			const RenderSpriteChain& spriteChain = static_cast<const RenderSpriteChain&>(*elements[i]);

			const VertexDeclaration* vertexDeclaration = spriteChain.GetVertexDeclaration();
			std::size_t spriteStride = 4 * vertexDeclaration->GetStride();

			if (m_pendingData.currentVertexDeclaration != vertexDeclaration)
			{
				// TODO: It's possible to use another vertex declaration with the same vertex buffer but currently very complicated
				// Wait until buffer rewrite
				Flush();
				m_pendingData.currentVertexDeclaration = vertexDeclaration;
			}

			m_pendingData.currentPipeline = &spriteChain.GetRenderPipeline();

			if (const MaterialProxy* materialProxy = &spriteChain.GetMaterialProxy(); m_pendingData.currentMaterialProxy != materialProxy)
			{
				m_pendingData.currentMaterialProxy = materialProxy;
				m_pendingData.currentMaterialProxy->FillRenderResourceReferences(*data.references);
				m_pendingData.currentMaterialShaderBinding = nullptr;
			}

			const Recti& scissorBox = spriteChain.GetScissorBox();
			m_pendingData.currentScissorBox = (scissorBox.width >= 0) ? scissorBox : invalidScissorBox;

			std::size_t spriteCount = spriteChain.GetSpriteCount();

			UInt64 requiredMemory = spriteCount * spriteStride;
			UInt64 remainingMemory = (m_pendingData.currentAllocation) ? SafeCast<UInt64>(m_pendingData.currentAllocationMemPtr - static_cast<UInt8*>(m_pendingData.currentAllocation->mappedPtr)) : 0;
			if (requiredMemory > remainingMemory)
			{
				Flush();

				m_pendingData.currentAllocation = &renderResources.GetUploadPool().Allocate(VertexBufferSize);
				m_pendingData.currentAllocationMemPtr = static_cast<UInt8*>(m_pendingData.currentAllocation->mappedPtr);

				std::shared_ptr<GpuBuffer> vertexBuffer;

				// Reuse vertex buffers from pool if any
				if (!m_pool->vertexBuffers.empty())
				{
					vertexBuffer = std::move(m_pool->vertexBuffers.back());
					m_pool->vertexBuffers.pop_back();
				}
				else
					vertexBuffer = m_device.InstantiateBuffer(VertexBufferSize, BufferUsage::VertexBuffer | BufferUsage::DeviceLocal);

				m_pendingData.currentVertexBuffer = vertexBuffer.get();

				data.vertexBuffers.emplace_back(std::move(vertexBuffer));
			}

			if (!m_pendingData.currentSceneShaderBinding || m_pendingData.sceneSetHash != m_pendingData.currentMaterialProxy->GetBindingSetHash(Material::SceneBindingSet))
			{
				m_pendingData.sceneSetHash = m_pendingData.currentMaterialProxy->GetBindingSetHash(Material::SceneBindingSet);

				ShaderBinding* sceneBinding = renderData.shaderBindingCache->GetSceneBinding(m_pendingData.sceneSetHash, [&]
				{
					ShaderBindingPtr sceneBinding = m_pendingData.currentPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(Material::SceneBindingSet);

					m_bindingCache.clear();
					m_pendingData.currentMaterialProxy->FillSceneBindings(sceneData, m_bindingCache);
					sceneBinding->Update(m_bindingCache.data(), m_bindingCache.size());

					return sceneBinding;
				});

				m_pendingData.currentSceneShaderBinding = sceneBinding;
				m_pendingData.currentViewerShaderBinding = nullptr;
			}

			if (!m_pendingData.currentViewerShaderBinding || m_pendingData.viewerSetHash != m_pendingData.currentMaterialProxy->GetBindingSetHash(Material::ViewerBindingSet))
			{
				m_pendingData.viewerSetHash = m_pendingData.currentMaterialProxy->GetBindingSetHash(Material::ViewerBindingSet);

				ShaderBinding* viewerBinding = renderData.shaderBindingCache->GetViewerBinding(viewer.GetViewerInstance(), m_pendingData.viewerSetHash, [&]
				{
					ShaderBindingPtr viewerBinding = m_pendingData.currentPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(Material::ViewerBindingSet);

					m_bindingCache.clear();
					m_pendingData.currentMaterialProxy->FillViewerBindings(viewer, m_bindingCache);
					viewerBinding->Update(m_bindingCache.data(), m_bindingCache.size());

					return viewerBinding;
				});

				m_pendingData.currentMaterialShaderBinding = nullptr;
				m_pendingData.currentViewerShaderBinding = viewerBinding;
			}

			if (!m_pendingData.currentMaterialShaderBinding)
				m_pendingData.currentMaterialShaderBinding = &m_pendingData.currentMaterialProxy->GetShaderBinding(renderResources);

			data.drawCalls.push_back(SpriteChainRendererData::DrawCall{
				.vertexBuffer = m_pendingData.currentVertexBuffer,
				.renderPipeline = m_pendingData.currentPipeline,
				.materialShaderBinding = m_pendingData.currentMaterialShaderBinding,
				.sceneShaderBinding = m_pendingData.currentSceneShaderBinding,
				.viewerShaderBinding = m_pendingData.currentViewerShaderBinding,
				.firstIndex = 6 * m_pendingData.firstQuadIndex,
				.indexCount = 6 * spriteCount,
				.scissorBox = m_pendingData.currentScissorBox,
				.instanceIndex = spriteChain.GetInstanceIndex()
			});

			const UInt8* spriteData = static_cast<const UInt8*>(spriteChain.GetSpriteData());

			std::memcpy(m_pendingData.currentAllocationMemPtr, spriteData, requiredMemory);
			m_pendingData.currentAllocationMemPtr += requiredMemory;
			m_pendingData.firstQuadIndex += spriteCount;
		}

		const RenderSpriteChain* firstSpriteChain = static_cast<const RenderSpriteChain*>(elements[0]);
		std::size_t drawCallCount = data.drawCalls.size() - oldDrawCallCount;
		data.drawCallPerElement[firstSpriteChain] = SpriteChainRendererData::DrawCallIndices{ oldDrawCallCount, drawCallCount };
	}

	void SpriteChainRenderer::PrepareEnd(ElementRendererData& /*rendererData*/, GpuResources& /*renderResources*/, GpuCommandBufferBuilder& commandBuffer)
	{
		Flush();

		if (!m_pendingCopies.empty())
		{
			for (auto& copy : m_pendingCopies)
				commandBuffer.CopyBuffer(*copy.allocation, copy.targetBuffer, copy.size);

			commandBuffer.MemoryBarrier({ .srcStageMask = PipelineStage::Transfer, .dstStageMask = PipelineStage::VertexInput, .srcAccessMask = MemoryAccess::TransferWrite, .dstAccessMask = MemoryAccess::VertexBufferRead });

			m_pendingCopies.clear();
		}

		m_pendingData = PendingData{};
	}

	void SpriteChainRenderer::Render(const RenderData& renderData, const SceneData& /*sceneData*/, const AbstractViewer& /*viewer*/, ElementRendererData& rendererData, GpuResources& /*renderResources*/, GpuCommandBufferBuilder& commandBuffer, std::size_t /*elementCount*/, const Pointer<const RenderElement>* elements)
	{
		auto& data = static_cast<SpriteChainRendererData&>(rendererData);

		commandBuffer.BindIndexBuffer(*m_indexBuffer, Nz::IndexType::U16);

		const GpuBuffer* currentVertexBuffer = nullptr;
		const GpuRenderPipeline* currentPipeline = nullptr;
		const ShaderBinding* currentMaterialShaderBinding = nullptr;
		const ShaderBinding* currentSceneShaderBinding = nullptr;
		const ShaderBinding* currentViewerShaderBinding = nullptr;
		Recti currentScissorBox(-1, -1, -1, -1);

		const RenderSpriteChain* firstSpriteChain = static_cast<const RenderSpriteChain*>(elements[0]);
		auto it = data.drawCallPerElement.find(firstSpriteChain);
		NazaraAssert(it != data.drawCallPerElement.end());

		const auto& indices = it->second;

		for (std::size_t i = 0; i < indices.count; ++i)
		{
			const auto& drawData = data.drawCalls[indices.start + i];

			if (currentVertexBuffer != drawData.vertexBuffer)
			{
				commandBuffer.BindVertexBuffer(0, *drawData.vertexBuffer);
				currentVertexBuffer = drawData.vertexBuffer;
			}

			if (currentPipeline != drawData.renderPipeline)
			{
				commandBuffer.BindRenderPipeline(*drawData.renderPipeline);
				currentPipeline = drawData.renderPipeline;
			}

			if (currentSceneShaderBinding != drawData.sceneShaderBinding)
			{
				commandBuffer.BindRenderShaderBinding(Material::SceneBindingSet, *drawData.sceneShaderBinding);

				currentSceneShaderBinding = drawData.sceneShaderBinding;
				currentViewerShaderBinding = nullptr;
			}

			if (currentViewerShaderBinding != drawData.viewerShaderBinding)
			{
				commandBuffer.BindRenderShaderBinding(Material::ViewerBindingSet, *drawData.viewerShaderBinding);

				currentMaterialShaderBinding = nullptr;
				currentViewerShaderBinding = drawData.viewerShaderBinding;
			}

			if (currentMaterialShaderBinding != drawData.materialShaderBinding)
			{
				commandBuffer.BindRenderShaderBinding(Material::MaterialBindingSet, *drawData.materialShaderBinding);

				currentMaterialShaderBinding = drawData.materialShaderBinding;
			}

			const Recti& targetScissorBox = (drawData.scissorBox.width >= 0) ? drawData.scissorBox : renderData.renderRegion;
			if (currentScissorBox != targetScissorBox)
			{
				commandBuffer.SetScissor(targetScissorBox);
				currentScissorBox = targetScissorBox;
			}

			commandBuffer.DrawIndexed(SafeCaster(drawData.indexCount), 1U, SafeCaster(drawData.firstIndex), 0, drawData.instanceIndex);
		}
	}

	void SpriteChainRenderer::Reset(ElementRendererData& rendererData, GpuResources& renderResources)
	{
		auto& data = static_cast<SpriteChainRendererData&>(rendererData);

		if (data.references)
		{
			renderResources.PushReleaseCallback([pool = m_pool, references = std::move(*data.references)]() mutable
			{
				references.Clear();
				pool->references.push_back(std::move(references));
			});
			data.references.reset();
		}

		for (auto& vertexBufferPtr : data.vertexBuffers)
		{
			renderResources.PushReleaseCallback([pool = m_pool, vertexBuffer = std::move(vertexBufferPtr)]() mutable
			{
				pool->vertexBuffers.push_back(std::move(vertexBuffer));
			});
		}
		data.vertexBuffers.clear();

		for (auto& shaderBinding : data.shaderBindings)
			renderResources.PushForRelease(std::move(shaderBinding));
		data.shaderBindings.clear();

		data.drawCalls.clear();
	}

	void SpriteChainRenderer::Flush()
	{
		if (m_pendingData.currentAllocation)
		{
			std::size_t size = m_pendingData.currentAllocationMemPtr - static_cast<UInt8*>(m_pendingData.currentAllocation->mappedPtr);

			m_pendingCopies.emplace_back(BufferCopy{
				m_pendingData.currentVertexBuffer,
				m_pendingData.currentAllocation,
				size
			});

			m_pendingData.firstQuadIndex = 0;
			m_pendingData.currentAllocation = nullptr;
			m_pendingData.currentVertexBuffer = nullptr;
		}
	}
}
