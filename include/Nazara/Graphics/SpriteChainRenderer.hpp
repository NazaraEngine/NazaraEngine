// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SPRITECHAINRENDERER_HPP
#define NAZARA_GRAPHICS_SPRITECHAINRENDERER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/RenderSpriteChain.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Nz
{
	class RenderDevice;
	class RenderPipeline;
	class ShaderBinding;
	class Texture;
	class TextureAsset;
	class VertexDeclaration;
	class WorldInstance;

	struct SpriteChainRendererData : public ElementRendererData
	{
		struct DrawCall
		{
			const RenderBuffer* vertexBuffer;
			const RenderPipeline* renderPipeline;
			const ShaderBinding* shaderBinding;
			std::size_t firstIndex;
			std::size_t quadCount;
			Recti scissorBox;
		};

		struct DrawCallIndices
		{
			std::size_t start;
			std::size_t count;
		};

		std::unordered_map<const RenderSpriteChain*, DrawCallIndices> drawCallPerElement;
		std::vector<DrawCall> drawCalls;
		std::vector<std::shared_ptr<RenderBuffer>> vertexBuffers;
		std::vector<ShaderBindingPtr> shaderBindings;
	};

	class NAZARA_GRAPHICS_API SpriteChainRenderer final : public ElementRenderer
	{
		public:
			SpriteChainRenderer(RenderDevice& device, std::size_t maxVertexBufferSize = 32 * 1024);
			~SpriteChainRenderer() = default;

			RenderElementPool<RenderSpriteChain>& GetPool() override;

			std::unique_ptr<ElementRendererData> InstanciateData() override;
			void Prepare(const ViewerInstance& viewerInstance, ElementRendererData& rendererData, RenderResources& currentFrame, std::size_t elementCount, const Pointer<const RenderElement>* elements, SparsePtr<const RenderStates> renderStates) override;
			void PrepareEnd(RenderResources& renderResources, ElementRendererData& rendererData) override;
			void Render(const ViewerInstance& viewerInstance, ElementRendererData& rendererData, CommandBufferBuilder& commandBuffer, std::size_t elementCount, const Pointer<const RenderElement>* elements) override;
			void Reset(ElementRendererData& rendererData, RenderResources& currentFrame) override;

		private:
			void Flush();
			void FlushDrawCall();
			void FlushDrawData();

			struct BufferCopy
			{
				RenderBuffer* targetBuffer;
				UploadPool::Allocation* allocation;
				std::size_t size;
			};

			struct PendingData
			{
				std::size_t firstQuadIndex = 0;
				SpriteChainRendererData::DrawCall* currentDrawCall = nullptr;
				UploadPool::Allocation* currentAllocation = nullptr;
				UInt8* currentAllocationMemPtr = nullptr;
				const VertexDeclaration* currentVertexDeclaration = nullptr;
				RenderBuffer* currentVertexBuffer = nullptr;
				const MaterialInstance* currentMaterialInstance = nullptr;
				const RenderPipeline* currentPipeline = nullptr;
				const ShaderBinding* currentShaderBinding = nullptr;
				const Texture* currentTextureOverlay = nullptr;
				const TextureAsset* currentTextureAssetOverlay = nullptr;
				const WorldInstance* currentWorldInstance = nullptr;
				RenderBufferView currentLightData;
				Recti currentScissorBox = Recti(-1, -1, -1, -1);
			};

			struct VertexBufferPool
			{
				std::vector<std::shared_ptr<RenderBuffer>> vertexBuffers;
			};

			std::shared_ptr<RenderBuffer> m_indexBuffer;
			std::shared_ptr<VertexBufferPool> m_vertexBufferPool;
			std::size_t m_maxVertexBufferSize;
			std::size_t m_maxVertexCount;
			std::vector<BufferCopy> m_pendingCopies;
			std::vector<ShaderBinding::Binding> m_bindingCache;
			RenderElementPool<RenderSpriteChain> m_spriteChainPool;
			PendingData m_pendingData;
			RenderDevice& m_device;
	};
}

#include <Nazara/Graphics/SpriteChainRenderer.inl>

#endif // NAZARA_GRAPHICS_SPRITECHAINRENDERER_HPP
