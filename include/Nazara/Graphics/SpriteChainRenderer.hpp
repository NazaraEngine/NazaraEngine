// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SPRITECHAINRENDERER_HPP
#define NAZARA_GRAPHICS_SPRITECHAINRENDERER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Nz
{
	class AbstractBuffer;
	class RenderDevice;
	class RenderPipeline;
	class RenderSpriteChain;
	class ShaderBinding;

	class NAZARA_GRAPHICS_API SpriteChainRenderer : public ElementRenderer
	{
		public:
			SpriteChainRenderer(RenderDevice& device, std::size_t maxVertexBufferSize = 32 * 1024);
			~SpriteChainRenderer() = default;

			std::unique_ptr<ElementRendererData> InstanciateData() override;
			void Prepare(const ViewerInstance& viewerInstance, ElementRendererData& rendererData, RenderFrame& currentFrame, const Pointer<const RenderElement>* elements, std::size_t elementCount) override;
			void Render(const ViewerInstance& viewerInstance, ElementRendererData& rendererData, CommandBufferBuilder& commandBuffer, const Pointer<const RenderElement>* elements, std::size_t elementCount) override;
			void Reset(ElementRendererData& rendererData, RenderFrame& currentFrame) override;

		private:
			struct BufferCopy
			{
				AbstractBuffer* targetBuffer;
				UploadPool::Allocation* allocation;
				std::size_t size;
			};

			struct VertexBufferPool
			{
				std::vector<std::shared_ptr<AbstractBuffer>> vertexBuffers;
			};

			std::shared_ptr<AbstractBuffer> m_indexBuffer;
			std::shared_ptr<VertexBufferPool> m_vertexBufferPool;
			std::size_t m_maxVertexBufferSize;
			std::size_t m_maxVertexCount;
			std::vector<BufferCopy> m_pendingCopies;
			std::vector<ShaderBinding::Binding> m_bindingCache;
			RenderDevice& m_device;
	};

	struct SpriteChainRendererData : public ElementRendererData
	{
		struct DrawCall
		{
			const AbstractBuffer* vertexBuffer;
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
		std::vector<std::shared_ptr<AbstractBuffer>> vertexBuffers;
		std::vector<ShaderBindingPtr> shaderBindings;
	};
}

#include <Nazara/Graphics/SpriteChainRenderer.inl>

#endif // NAZARA_GRAPHICS_SPRITECHAINRENDERER_HPP
