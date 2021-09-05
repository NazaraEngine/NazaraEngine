// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/SpriteChainRenderer.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/RenderSpriteChain.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <utility>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	SpriteChainRenderer::SpriteChainRenderer(RenderDevice& device, std::size_t maxVertexBufferSize) :
	m_device(device),
	m_maxVertexBufferSize(maxVertexBufferSize),
	m_maxVertexCount(m_maxVertexBufferSize / (2 * sizeof(float))) // Treat vec2 as the minimum declaration possible
	{
		std::size_t maxQuadCount = m_maxVertexCount / 4;
		std::size_t indexCount = 6 * maxQuadCount;

		m_indexBuffer = m_device.InstantiateBuffer(BufferType::Index);
		if (!m_indexBuffer->Initialize(indexCount * sizeof(UInt16), BufferUsage::DeviceLocal))
			throw std::runtime_error("failed to initialize index buffer");
		
		// Generate indices for quad (0, 1, 2, 2, 1, 3, ...)
		std::vector<UInt16> indices(indexCount);
		UInt16* indexPtr = indices.data();

		for (std::size_t i = 0; i < maxQuadCount; ++i)
		{
			*indexPtr++ = i * 4 + 0;
			*indexPtr++ = i * 4 + 1;
			*indexPtr++ = i * 4 + 2;

			*indexPtr++ = i * 4 + 2;
			*indexPtr++ = i * 4 + 1;
			*indexPtr++ = i * 4 + 3;
		}

		m_indexBuffer->Fill(indices.data(), 0, indexCount * sizeof(UInt16));
	}

	std::unique_ptr<ElementRendererData> SpriteChainRenderer::InstanciateData()
	{
		return std::make_unique<SpriteChainRendererData>();
	}

	void SpriteChainRenderer::Prepare(ElementRendererData& rendererData, RenderFrame& currentFrame, const Pointer<const RenderElement>* elements, std::size_t elementCount)
	{
		auto& data = static_cast<SpriteChainRendererData&>(rendererData);

		std::vector<std::pair<UploadPool::Allocation*, AbstractBuffer*>> pendingCopies;

		std::size_t firstQuadIndex = 0;
		SpriteChainRendererData::DrawCall* currentDrawCall = nullptr;
		UploadPool::Allocation* currentAllocation = nullptr;
		UInt8* currentAllocationMemPtr = nullptr;
		const VertexDeclaration* currentVertexDeclaration = nullptr;
		AbstractBuffer* currentVertexBuffer = nullptr;
		const RenderPipeline* currentPipeline = nullptr;
		const ShaderBinding* currentInstanceBinding = nullptr;
		const ShaderBinding* currentMaterialBinding = nullptr;

		auto FlushDrawCall = [&]()
		{
			currentDrawCall = nullptr;
		};

		auto Flush = [&]()
		{
			// changing vertex buffer always mean we have to switch draw calls
			FlushDrawCall();

			if (currentAllocation)
			{
				pendingCopies.emplace_back(currentAllocation, currentVertexBuffer);

				firstQuadIndex = 0;
				currentAllocation = nullptr;
				currentVertexBuffer = nullptr;
			}
		};

		std::size_t oldDrawCallCount = data.drawCalls.size();

		for (std::size_t i = 0; i < elementCount; ++i)
		{
			assert(elements[i]->GetElementType() == UnderlyingCast(BasicRenderElement::SpriteChain));
			const RenderSpriteChain& spriteChain = static_cast<const RenderSpriteChain&>(*elements[i]);

			const VertexDeclaration* vertexDeclaration = spriteChain.GetVertexDeclaration();
			std::size_t stride = vertexDeclaration->GetStride();

			if (currentVertexDeclaration != vertexDeclaration)
			{
				// TODO: It's be possible to use another vertex declaration with the same vertex buffer but currently very complicated
				// Wait until buffer rewrite
				Flush();
				currentVertexDeclaration = vertexDeclaration;
			}

			if (currentPipeline != spriteChain.GetRenderPipeline())
			{
				FlushDrawCall();
				currentPipeline = spriteChain.GetRenderPipeline();
			}

			if (currentMaterialBinding != &spriteChain.GetMaterialBinding())
			{
				FlushDrawCall();
				currentMaterialBinding = &spriteChain.GetMaterialBinding();
			}

			if (currentInstanceBinding != &spriteChain.GetInstanceBinding())
			{
				// TODO: Flushing draw calls on instance binding means we can have e.g. 1000 sprites rendered using a draw call for each one
				// which is far from being efficient, using some bindless could help (or at least instancing?)
				FlushDrawCall();
				currentInstanceBinding = &spriteChain.GetInstanceBinding();
			}

			std::size_t remainingQuads = spriteChain.GetSpriteCount();
			while (remainingQuads > 0)
			{
				if (!currentAllocation)
				{
					currentAllocation = &currentFrame.GetUploadPool().Allocate(m_maxVertexBufferSize);
					currentAllocationMemPtr = static_cast<UInt8*>(currentAllocation->mappedPtr);

					std::shared_ptr<AbstractBuffer> vertexBuffer = m_device.InstantiateBuffer(BufferType::Vertex);
					vertexBuffer->Initialize(m_maxVertexBufferSize, BufferUsage::DeviceLocal);

					currentVertexBuffer = vertexBuffer.get();

					data.vertexBuffers.emplace_back(std::move(vertexBuffer));
				}

				if (!currentDrawCall)
				{
					data.drawCalls.push_back(SpriteChainRendererData::DrawCall{
						currentVertexBuffer,
						currentPipeline,
						currentInstanceBinding,
						currentMaterialBinding,
						6 * firstQuadIndex,
						0,
					});

					currentDrawCall = &data.drawCalls.back();
				}

				std::size_t remainingSpace = m_maxVertexBufferSize - (currentAllocationMemPtr - currentAllocation->mappedPtr);
				std::size_t maxQuads = remainingSpace / (4 * stride);
				if (maxQuads == 0)
				{
					Flush();
					continue;
				}

				std::size_t copiedQuadCount = std::min(maxQuads, remainingQuads);
				std::size_t copiedSize = 4 * copiedQuadCount * stride;

				std::memcpy(currentAllocationMemPtr, spriteChain.GetSpriteData(), copiedSize);
				currentAllocationMemPtr += copiedSize;

				firstQuadIndex += copiedQuadCount;
				currentDrawCall->quadCount += copiedQuadCount;
				remainingQuads -= copiedQuadCount;

				// If there's still data to copy, it means buffer is full, flush it
				if (remainingQuads > 0)
					Flush();
			}
		}

		//TODO: Add Finish()/PrepareEnd() call to allow to reuse buffers/draw calls for multiple Prepare calls
		Flush();

		const RenderSpriteChain* firstSpriteChain = static_cast<const RenderSpriteChain*>(elements[0]);
		std::size_t drawCallCount = data.drawCalls.size() - oldDrawCallCount;
		data.drawCallPerElement[firstSpriteChain] = SpriteChainRendererData::DrawCallIndices{ oldDrawCallCount, drawCallCount };

		if (!pendingCopies.empty())
		{
			currentFrame.Execute([&](CommandBufferBuilder& builder)
			{
				for (auto&& [allocation, buffer] : pendingCopies)
					builder.CopyBuffer(*allocation, buffer);

				builder.PostTransferBarrier();
			}, Nz::QueueType::Transfer);
		}
	}

	void SpriteChainRenderer::Render(ElementRendererData& rendererData, CommandBufferBuilder& commandBuffer, const Pointer<const RenderElement>* elements, std::size_t elementCount)
	{
		auto& data = static_cast<SpriteChainRendererData&>(rendererData);

		commandBuffer.BindIndexBuffer(*m_indexBuffer);

		const AbstractBuffer* currentVertexBuffer = nullptr;
		const RenderPipeline* currentPipeline = nullptr;
		const ShaderBinding* currentInstanceBinding = nullptr;
		const ShaderBinding* currentMaterialBinding = nullptr;

		const RenderSpriteChain* firstSpriteChain = static_cast<const RenderSpriteChain*>(elements[0]);
		auto it = data.drawCallPerElement.find(firstSpriteChain);
		assert(it != data.drawCallPerElement.end());

		const auto& indices = it->second;

		for (std::size_t i = 0; i < indices.count; ++i)
		{
			const auto& drawCall = data.drawCalls[indices.start + i];

			if (currentVertexBuffer != drawCall.vertexBuffer)
			{
				commandBuffer.BindVertexBuffer(0, *drawCall.vertexBuffer);
				currentVertexBuffer = drawCall.vertexBuffer;
			}

			if (currentPipeline != drawCall.renderPipeline)
			{
				commandBuffer.BindPipeline(*drawCall.renderPipeline);
				currentPipeline = drawCall.renderPipeline;
			}

			if (currentMaterialBinding != drawCall.materialBinding)
			{
				commandBuffer.BindShaderBinding(Graphics::MaterialBindingSet, *drawCall.materialBinding);
				currentMaterialBinding = drawCall.materialBinding;
			}

			if (currentInstanceBinding != drawCall.instanceBinding)
			{
				commandBuffer.BindShaderBinding(Graphics::WorldBindingSet, *drawCall.instanceBinding);
				currentInstanceBinding = drawCall.instanceBinding;
			}

			commandBuffer.DrawIndexed(drawCall.quadCount * 6, 1U, drawCall.firstIndex);
		}
	}

	void SpriteChainRenderer::Reset(ElementRendererData& rendererData, RenderFrame& currentFrame)
	{
		auto& data = static_cast<SpriteChainRendererData&>(rendererData);

		// TODO: Reuse vertex buffers
		for (auto& vertexBufferPtr : data.vertexBuffers)
			currentFrame.PushForRelease(std::move(vertexBufferPtr));

		data.drawCalls.clear();
		data.vertexBuffers.clear();
	}
}
