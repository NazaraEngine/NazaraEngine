// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/SpriteChainRenderer.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/RenderSpriteChain.hpp>
#include <Nazara/Graphics/TextureAsset.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderResources.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <iostream>
#include <utility>

namespace Nz
{
	SpriteChainRenderer::SpriteChainRenderer(RenderDevice& device, std::size_t maxVertexBufferSize) :
	m_maxVertexBufferSize(maxVertexBufferSize),
	m_maxVertexCount(m_maxVertexBufferSize / (2 * sizeof(float))), // Treat vec2 as the minimum declaration possible
	m_device(device)
	{
		m_vertexBufferPool = std::make_shared<VertexBufferPool>();

		std::size_t maxQuadCount = m_maxVertexCount / 4;
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

		m_indexBuffer = m_device.InstantiateBuffer(BufferType::Index, indexCount * sizeof(UInt16), BufferUsage::DeviceLocal | BufferUsage::Write, indices.data());
	}

	RenderElementPool<RenderSpriteChain>& SpriteChainRenderer::GetPool()
	{
		return m_spriteChainPool;
	}

	std::unique_ptr<ElementRendererData> SpriteChainRenderer::InstanciateData()
	{
		return std::make_unique<SpriteChainRendererData>();
	}

	void SpriteChainRenderer::Prepare(const ViewerInstance& viewerInstance, ElementRendererData& rendererData, RenderResources& renderResources, std::size_t elementCount, const Pointer<const RenderElement>* elements, SparsePtr<const RenderStates> renderStates)
	{
		Graphics* graphics = Graphics::Instance();

		const auto& defaultSampler = graphics->GetSamplerCache().Get({});

		auto& data = static_cast<SpriteChainRendererData&>(rendererData);

		Recti invalidScissorBox(-1, -1, -1, -1);

		std::size_t oldDrawCallCount = data.drawCalls.size();

		for (std::size_t i = 0; i < elementCount; ++i)
		{
			assert(elements[i]->GetElementType() == UnderlyingCast(BasicRenderElement::SpriteChain));
			const RenderSpriteChain& spriteChain = static_cast<const RenderSpriteChain&>(*elements[i]);
			const RenderStates& renderState = renderStates[i];

			const VertexDeclaration* vertexDeclaration = spriteChain.GetVertexDeclaration();
			std::size_t stride = vertexDeclaration->GetStride();

			if (m_pendingData.currentVertexDeclaration != vertexDeclaration)
			{
				// TODO: It's be possible to use another vertex declaration with the same vertex buffer but currently very complicated
				// Wait until buffer rewrite
				Flush();
				m_pendingData.currentVertexDeclaration = vertexDeclaration;
			}

			if (const RenderPipeline* pipeline = &spriteChain.GetRenderPipeline(); m_pendingData.currentPipeline != pipeline)
			{
				FlushDrawData();
				m_pendingData.currentPipeline = pipeline;
			}

			if (const MaterialInstance* materialInstance = &spriteChain.GetMaterialInstance(); m_pendingData.currentMaterialInstance != materialInstance)
			{
				FlushDrawData();
				m_pendingData.currentMaterialInstance = materialInstance;
			}

			if (const WorldInstance* worldInstance = &spriteChain.GetWorldInstance(); m_pendingData.currentWorldInstance != worldInstance)
			{
				// TODO: Flushing draw calls on instance binding means we can have e.g. 1000 sprites rendered using a draw call for each one
				// which is far from being efficient, using some bindless could help (or at least instancing?)
				FlushDrawData();
				m_pendingData.currentWorldInstance = worldInstance;
			}

			if (const TextureAsset* textureOverlay = spriteChain.GetTextureOverlay(); m_pendingData.currentTextureAssetOverlay != textureOverlay)
			{
				FlushDrawData();
				m_pendingData.currentTextureAssetOverlay = textureOverlay;
				m_pendingData.currentTextureOverlay = textureOverlay->GetOrCreateTexture(m_device).get();
			}

			if (m_pendingData.currentLightData != renderState.lightData)
			{
				FlushDrawData();
				m_pendingData.currentLightData = renderState.lightData;
			}

			const Recti& scissorBox = spriteChain.GetScissorBox();
			const Recti& targetScissorBox = (scissorBox.width >= 0) ? scissorBox : invalidScissorBox;
			if (m_pendingData.currentScissorBox != targetScissorBox)
			{
				FlushDrawCall();
				m_pendingData.currentScissorBox = targetScissorBox;
			}

			std::size_t remainingQuads = spriteChain.GetSpriteCount();
			const UInt8* spriteData = static_cast<const UInt8*>(spriteChain.GetSpriteData());

			while (remainingQuads > 0)
			{
				if (!m_pendingData.currentAllocation)
				{
					m_pendingData.currentAllocation = &renderResources.GetUploadPool().Allocate(m_maxVertexBufferSize);
					m_pendingData.currentAllocationMemPtr = static_cast<UInt8*>(m_pendingData.currentAllocation->mappedPtr);

					std::shared_ptr<RenderBuffer> vertexBuffer;

					// Try to reuse vertex buffers from pool if any
					if (!m_vertexBufferPool->vertexBuffers.empty())
					{
						vertexBuffer = std::move(m_vertexBufferPool->vertexBuffers.back());
						m_vertexBufferPool->vertexBuffers.pop_back();
					}
					else
						vertexBuffer = m_device.InstantiateBuffer(BufferType::Vertex, m_maxVertexBufferSize, BufferUsage::DeviceLocal | BufferUsage::Dynamic | BufferUsage::Write);

					m_pendingData.currentVertexBuffer = vertexBuffer.get();

					data.vertexBuffers.emplace_back(std::move(vertexBuffer));
				}

				if (!m_pendingData.currentShaderBinding)
				{
					m_bindingCache.clear();

					const MaterialInstance& materialInstance = spriteChain.GetMaterialInstance();
					materialInstance.FillShaderBinding(m_bindingCache);

					// Engine shader bindings
					const Material& material = *materialInstance.GetParentMaterial();

					if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::InstanceDataUbo); bindingIndex != Material::InvalidBindingIndex)
					{
						const auto& instanceBuffer = m_pendingData.currentWorldInstance->GetInstanceBuffer();

						auto& bindingEntry = m_bindingCache.emplace_back();
						bindingEntry.bindingIndex = bindingIndex;
						bindingEntry.content = ShaderBinding::UniformBufferBinding{
							instanceBuffer.get(),
							0, instanceBuffer->GetSize()
						};
					}

					if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::LightDataUbo); bindingIndex != Material::InvalidBindingIndex && m_pendingData.currentLightData)
					{
						auto& bindingEntry = m_bindingCache.emplace_back();
						bindingEntry.bindingIndex = bindingIndex;
						bindingEntry.content = ShaderBinding::UniformBufferBinding{
							m_pendingData.currentLightData.GetBuffer(),
							m_pendingData.currentLightData.GetOffset(), m_pendingData.currentLightData.GetSize()
						};
					}

					if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::ViewerDataUbo); bindingIndex != Material::InvalidBindingIndex)
					{
						const auto& viewerBuffer = viewerInstance.GetViewerBuffer();

						auto& bindingEntry = m_bindingCache.emplace_back();
						bindingEntry.bindingIndex = bindingIndex;
						bindingEntry.content = ShaderBinding::UniformBufferBinding{
							viewerBuffer.get(),
							0, viewerBuffer->GetSize()
						};
					}

					if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::OverlayTexture); bindingIndex != Material::InvalidBindingIndex)
					{
						auto& bindingEntry = m_bindingCache.emplace_back();
						bindingEntry.bindingIndex = bindingIndex;
						bindingEntry.content = ShaderBinding::SampledTextureBinding{
							m_pendingData.currentTextureOverlay, defaultSampler.get()
						};
					}

					ShaderBindingPtr drawDataBinding = m_pendingData.currentPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(0);
					drawDataBinding->Update(m_bindingCache.data(), m_bindingCache.size());

					m_pendingData.currentShaderBinding = drawDataBinding.get();

					data.shaderBindings.emplace_back(std::move(drawDataBinding));
				}

				if (!m_pendingData.currentDrawCall)
				{
					data.drawCalls.push_back(SpriteChainRendererData::DrawCall{
						m_pendingData.currentVertexBuffer,
						m_pendingData.currentPipeline,
						m_pendingData.currentShaderBinding,
						6 * m_pendingData.firstQuadIndex,
						0,
						m_pendingData.currentScissorBox
					});

					m_pendingData.currentDrawCall = &data.drawCalls.back();
				}

				std::size_t remainingSpace = m_maxVertexBufferSize - (m_pendingData.currentAllocationMemPtr - static_cast<UInt8*>(m_pendingData.currentAllocation->mappedPtr));
				std::size_t maxQuads = remainingSpace / (4 * stride);
				if (maxQuads == 0)
				{
					Flush();
					continue;
				}

				std::size_t copiedQuadCount = std::min(maxQuads, remainingQuads);
				std::size_t copiedSize = 4 * copiedQuadCount * stride;

				std::memcpy(m_pendingData.currentAllocationMemPtr, spriteData, copiedSize);
				m_pendingData.currentAllocationMemPtr += copiedSize;
				spriteData += copiedSize;

				m_pendingData.firstQuadIndex += copiedQuadCount;
				m_pendingData.currentDrawCall->quadCount += copiedQuadCount;
				remainingQuads -= copiedQuadCount;

				// If there's still data to copy, it means buffer is full, flush it
				if (remainingQuads > 0)
					Flush();
			}
		}

		FlushDrawCall();

		const RenderSpriteChain* firstSpriteChain = static_cast<const RenderSpriteChain*>(elements[0]);
		std::size_t drawCallCount = data.drawCalls.size() - oldDrawCallCount;
		data.drawCallPerElement[firstSpriteChain] = SpriteChainRendererData::DrawCallIndices{ oldDrawCallCount, drawCallCount };
	}

	void SpriteChainRenderer::PrepareEnd(RenderResources& renderResources, ElementRendererData& /*rendererData*/)
	{
		Flush();

		if (!m_pendingCopies.empty())
		{
			renderResources.Execute([&](CommandBufferBuilder& builder)
			{
				for (auto& copy : m_pendingCopies)
					builder.CopyBuffer(*copy.allocation, copy.targetBuffer, copy.size);

				builder.MemoryBarrier(PipelineStage::Transfer, PipelineStage::VertexInput, MemoryAccess::TransferWrite, MemoryAccess::VertexBufferRead);
			}, Nz::QueueType::Transfer);

			m_pendingCopies.clear();
		}

		m_pendingData = PendingData{};
	}

	void SpriteChainRenderer::Render(const ViewerInstance& viewerInstance, ElementRendererData& rendererData, CommandBufferBuilder& commandBuffer, std::size_t /*elementCount*/, const Pointer<const RenderElement>* elements)
	{
		auto& data = static_cast<SpriteChainRendererData&>(rendererData);

		commandBuffer.BindIndexBuffer(*m_indexBuffer, Nz::IndexType::U16);

		Vector2f targetSize = viewerInstance.GetTargetSize();
		Recti fullscreenScissorBox(0, 0, SafeCast<int>(std::floor(targetSize.x)), SafeCast<int>(std::floor(targetSize.y)));

		const RenderBuffer* currentVertexBuffer = nullptr;
		const RenderPipeline* currentPipeline = nullptr;
		const ShaderBinding* currentShaderBinding = nullptr;
		Recti currentScissorBox(-1, -1, -1, -1);

		const RenderSpriteChain* firstSpriteChain = static_cast<const RenderSpriteChain*>(elements[0]);
		auto it = data.drawCallPerElement.find(firstSpriteChain);
		assert(it != data.drawCallPerElement.end());

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

			if (currentShaderBinding != drawData.shaderBinding)
			{
				commandBuffer.BindRenderShaderBinding(0, *drawData.shaderBinding);
				currentShaderBinding = drawData.shaderBinding;
			}

			const Recti& targetScissorBox = (drawData.scissorBox.width >= 0) ? drawData.scissorBox : fullscreenScissorBox;
			if (currentScissorBox != targetScissorBox)
			{
				commandBuffer.SetScissor(targetScissorBox);
				currentScissorBox = targetScissorBox;
			}

			commandBuffer.DrawIndexed(SafeCast<UInt32>(drawData.quadCount * 6), 1U, SafeCast<UInt32>(drawData.firstIndex));
		}
	}

	void SpriteChainRenderer::Reset(ElementRendererData& rendererData, RenderResources& renderResources)
	{
		auto& data = static_cast<SpriteChainRendererData&>(rendererData);

		for (auto& vertexBufferPtr : data.vertexBuffers)
		{
			renderResources.PushReleaseCallback([pool = m_vertexBufferPool, vertexBuffer = std::move(vertexBufferPtr)]() mutable
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
		// changing vertex buffer always mean we have to switch draw calls
		FlushDrawCall();

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

	void SpriteChainRenderer::FlushDrawCall()
	{
		m_pendingData.currentDrawCall = nullptr;
	}

	void SpriteChainRenderer::FlushDrawData()
	{
		FlushDrawCall();

		m_pendingData.currentShaderBinding = nullptr;
	}
}
