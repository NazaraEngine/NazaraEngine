// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SPRITECHAINRENDERER_HPP
#define NAZARA_GRAPHICS_SPRITECHAINRENDERER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/RenderResourceReferences.hpp>
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
			const ShaderBinding* materialShaderBinding;
			const ShaderBinding* sceneShaderBinding;
			const ShaderBinding* viewerShaderBinding;
			std::size_t firstIndex;
			std::size_t indexCount;
			Recti scissorBox;
			UInt32 instanceIndex;
		};

		struct DrawCallIndices
		{
			std::size_t start;
			std::size_t count;
		};

		std::optional<RenderResourceReferences> references;
		std::unordered_map<const RenderSpriteChain*, DrawCallIndices> drawCallPerElement;
		std::vector<DrawCall> drawCalls;
		std::vector<std::shared_ptr<RenderBuffer>> vertexBuffers;
		std::vector<ShaderBindingPtr> shaderBindings;
	};

	class NAZARA_GRAPHICS_API SpriteChainRenderer final : public ElementRenderer
	{
		public:
			SpriteChainRenderer(RenderDevice& device);
			~SpriteChainRenderer() = default;

			void ForEachIndirectBuffer(ElementRendererData& rendererData, FunctionRef<void(RenderBuffer* buffer, std::size_t commandCount)> callback) override;

			RenderElementPool<RenderSpriteChain>& GetPool() override;

			std::unique_ptr<ElementRendererData> InstanciateData() override;
			void Prepare(const RenderData& renderData, const SceneData& sceneData, const AbstractViewer& viewer, ElementRendererData& rendererData, RenderResources& currentFrame, std::size_t elementCount, const Pointer<const RenderElement>* elements) override;
			void PrepareEnd(ElementRendererData& rendererData, RenderResources& renderResources, CommandBufferBuilder& commandBuffer) override;
			void Render(const RenderData& renderData, const SceneData& sceneData, const AbstractViewer& viewer, ElementRendererData& rendererData, RenderResources& currentFrame, CommandBufferBuilder& commandBuffer, std::size_t elementCount, const Pointer<const RenderElement>* elements) override;
			void Reset(ElementRendererData& rendererData, RenderResources& currentFrame) override;

		private:
			void Flush();

			struct BufferCopy
			{
				RenderBuffer* targetBuffer;
				UploadPool::Allocation* allocation;
				std::size_t size;
			};

			struct PendingData
			{
				std::size_t firstQuadIndex = 0;
				std::size_t sceneSetHash = 0;
				std::size_t viewerSetHash = 0;
				UploadPool::Allocation* currentAllocation = nullptr;
				UInt8* currentAllocationMemPtr = nullptr;
				const VertexDeclaration* currentVertexDeclaration = nullptr;
				RenderBuffer* currentVertexBuffer = nullptr;
				const MaterialProxy* currentMaterialProxy = nullptr;
				const RenderPipeline* currentPipeline = nullptr;
				const ShaderBinding* currentMaterialShaderBinding = nullptr;
				const ShaderBinding* currentSceneShaderBinding = nullptr;
				const ShaderBinding* currentViewerShaderBinding = nullptr;
				Recti currentScissorBox = Recti(-1, -1, -1, -1);
			};

			struct PoolData
			{
				std::vector<RenderResourceReferences> references;
				std::vector<std::shared_ptr<RenderBuffer>> vertexBuffers;
			};

			std::shared_ptr<RenderBuffer> m_indexBuffer;
			std::shared_ptr<PoolData> m_pool;
			std::vector<BufferCopy> m_pendingCopies;
			std::vector<ShaderBinding::Binding> m_bindingCache;
			RenderElementPool<RenderSpriteChain> m_spriteChainPool;
			PendingData m_pendingData;
			RenderDevice& m_device;
	};
}

#include <Nazara/Graphics/SpriteChainRenderer.inl>

#endif // NAZARA_GRAPHICS_SPRITECHAINRENDERER_HPP
