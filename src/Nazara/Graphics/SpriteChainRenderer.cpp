// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/SpriteChainRenderer.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/RenderSpriteChain.hpp>
#include <Nazara/Graphics/TextureAsset.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderResources.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <utility>

namespace Nz
{
	SpriteChainRenderer::SpriteChainRenderer(RenderDevice& device) :
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

	RenderElementPool<RenderSpriteChain>& SpriteChainRenderer::GetPool()
	{
		return m_spriteChainPool;
	}

	std::unique_ptr<ElementRendererData> SpriteChainRenderer::InstanciateData()
	{
		return std::make_unique<SpriteChainRendererData>();
	}

	void SpriteChainRenderer::Prepare(const RenderData& renderData, const AbstractViewer& viewer, ElementRendererData& rendererData, RenderResources& renderResources, std::size_t elementCount, const Pointer<const RenderElement>* elements)
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

			if (const MaterialInstance* materialInstance = &spriteChain.GetMaterialInstance(); m_pendingData.currentMaterialInstance != materialInstance)
			{
				m_pendingData.currentMaterialShaderBinding = nullptr;
				m_pendingData.currentMaterialInstance = materialInstance;
			}

			if (const WorldInstance* worldInstance = &spriteChain.GetWorldInstance(); m_pendingData.currentWorldInstance != worldInstance)
			{
				// TODO: Flushing draw calls on instance binding means we can have e.g. 1000 sprites rendered using a draw call for each one
				// which is far from being efficient, using some bindless could help (or at least instancing?)
				m_pendingData.currentInstanceShaderBinding = nullptr;
				m_pendingData.currentWorldInstance = worldInstance;
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

				std::shared_ptr<RenderBuffer> vertexBuffer;

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

			const Material& material = *m_pendingData.currentMaterialInstance->GetParentMaterial();

			if (!m_pendingData.currentSceneShaderBinding)
			{
				ShaderBindingPtr sceneBinding = m_pendingData.currentPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(Material::SceneBindingSet);

				m_bindingCache.clear();
				FillSceneBindings(renderData, material, m_bindingCache);
				sceneBinding->Update(m_bindingCache.data(), m_bindingCache.size());

				m_pendingData.currentSceneShaderBinding = sceneBinding.get();

				data.shaderBindings.emplace_back(std::move(sceneBinding));

				m_pendingData.currentViewerShaderBinding = nullptr;
			}

			if (!m_pendingData.currentViewerShaderBinding)
			{
				ShaderBindingPtr viewerBinding = m_pendingData.currentPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(Material::ViewerBindingSet);

				m_bindingCache.clear();
				viewer.GetViewerInstance().FillShaderBinding(material, *data.references, m_bindingCache);
				viewerBinding->Update(m_bindingCache.data(), m_bindingCache.size());

				m_pendingData.currentViewerShaderBinding = viewerBinding.get();

				data.shaderBindings.emplace_back(std::move(viewerBinding));

				m_pendingData.currentMaterialShaderBinding = nullptr;
			}

			if (!m_pendingData.currentMaterialShaderBinding)
			{
				ShaderBindingPtr materialBinding = m_pendingData.currentPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(Material::MaterialBindingSet);

				m_bindingCache.clear();
				m_pendingData.currentMaterialInstance->FillShaderBinding(*data.references, m_bindingCache);
				materialBinding->Update(m_bindingCache.data(), m_bindingCache.size());

				m_pendingData.currentMaterialShaderBinding = materialBinding.get();

				data.shaderBindings.emplace_back(std::move(materialBinding));

				m_pendingData.currentInstanceShaderBinding = nullptr;
			}

			if (!m_pendingData.currentInstanceShaderBinding)
			{
				ShaderBindingPtr instanceBinding = m_pendingData.currentPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(Material::InstanceBindingSet);

				m_bindingCache.clear();
				m_pendingData.currentWorldInstance->FillShaderBinding(material, *data.references, m_bindingCache);

				instanceBinding->Update(m_bindingCache.data(), m_bindingCache.size());

				m_pendingData.currentInstanceShaderBinding = instanceBinding.get();

				data.shaderBindings.emplace_back(std::move(instanceBinding));
			}

			std::size_t sceneBindingHash = material.GetBindingSetHash(Material::SceneBindingSet);
			std::size_t viewerBindingHash = material.GetBindingSetHash(Material::ViewerBindingSet);

			data.drawCalls.push_back(SpriteChainRendererData::DrawCall{
				.vertexBuffer = m_pendingData.currentVertexBuffer,
				.renderPipeline = m_pendingData.currentPipeline,
				.instanceShaderBinding = m_pendingData.currentInstanceShaderBinding,
				.materialShaderBinding = m_pendingData.currentMaterialShaderBinding,
				.sceneShaderBinding = m_pendingData.currentSceneShaderBinding,
				.viewerShaderBinding = m_pendingData.currentViewerShaderBinding,
				.firstIndex = 6 * m_pendingData.firstQuadIndex,
				.indexCount = 6 * spriteCount,
				.sceneBindingHash = sceneBindingHash,
				.viewerBindingHash = viewerBindingHash,
				.scissorBox = m_pendingData.currentScissorBox
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

	void SpriteChainRenderer::PrepareEnd(ElementRendererData& /*rendererData*/, RenderResources& /*renderResources*/, CommandBufferBuilder& commandBuffer)
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

	void SpriteChainRenderer::Render(const RenderData& renderData, const AbstractViewer& /*viewer*/, ElementRendererData& rendererData, RenderResources& /*renderResources*/, CommandBufferBuilder& commandBuffer, std::size_t /*elementCount*/, const Pointer<const RenderElement>* elements)
	{
		auto& data = static_cast<SpriteChainRendererData&>(rendererData);

		commandBuffer.BindIndexBuffer(*m_indexBuffer, Nz::IndexType::U16);

		const RenderBuffer* currentVertexBuffer = nullptr;
		const RenderPipeline* currentPipeline = nullptr;
		const ShaderBinding* currentInstanceShaderBinding = nullptr;
		const ShaderBinding* currentMaterialShaderBinding = nullptr;
		const ShaderBinding* currentSceneShaderBinding = nullptr;
		const ShaderBinding* currentViewerShaderBinding = nullptr;
		Recti currentScissorBox(-1, -1, -1, -1);

		const RenderSpriteChain* firstSpriteChain = static_cast<const RenderSpriteChain*>(elements[0]);
		auto it = data.drawCallPerElement.find(firstSpriteChain);
		NazaraAssert(it != data.drawCallPerElement.end());

		const auto& indices = it->second;

		std::size_t sceneBindingHash = 0;
		std::size_t viewerBindingHash = 0;

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

			if (!currentSceneShaderBinding || sceneBindingHash != drawData.sceneBindingHash)
			{
				commandBuffer.BindRenderShaderBinding(Material::SceneBindingSet, *drawData.sceneShaderBinding);

				sceneBindingHash = drawData.sceneBindingHash;
				currentSceneShaderBinding = drawData.sceneShaderBinding;
				currentViewerShaderBinding = nullptr;
			}

			if (!currentViewerShaderBinding || viewerBindingHash != drawData.viewerBindingHash)
			{
				commandBuffer.BindRenderShaderBinding(Material::ViewerBindingSet, *drawData.viewerShaderBinding);

				viewerBindingHash = drawData.viewerBindingHash;
				currentMaterialShaderBinding = nullptr;
				currentViewerShaderBinding = drawData.viewerShaderBinding;
			}

			if (!currentMaterialShaderBinding)
			{
				commandBuffer.BindRenderShaderBinding(Material::MaterialBindingSet, *drawData.materialShaderBinding);

				currentInstanceShaderBinding = nullptr;
				currentMaterialShaderBinding = drawData.materialShaderBinding;
			}

			if (!currentInstanceShaderBinding)
			{
				commandBuffer.BindRenderShaderBinding(Material::InstanceBindingSet, *drawData.instanceShaderBinding);

				currentInstanceShaderBinding = drawData.instanceShaderBinding;
			}

			const Recti& targetScissorBox = (drawData.scissorBox.width >= 0) ? drawData.scissorBox : renderData.renderRegion;
			if (currentScissorBox != targetScissorBox)
			{
				commandBuffer.SetScissor(targetScissorBox);
				currentScissorBox = targetScissorBox;
			}

			commandBuffer.DrawIndexed(SafeCaster(drawData.indexCount), 1U, SafeCaster(drawData.firstIndex));
		}
	}

	void SpriteChainRenderer::Reset(ElementRendererData& rendererData, RenderResources& renderResources)
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
