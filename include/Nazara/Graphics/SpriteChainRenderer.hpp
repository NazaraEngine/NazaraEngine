// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPRITECHAINRENDERER_HPP
#define NAZARA_SPRITECHAINRENDERER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
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

			std::unique_ptr<ElementRendererData> InstanciateData();
			void Prepare(ElementRendererData& rendererData, RenderFrame& currentFrame, const Pointer<const RenderElement>* elements, std::size_t elementCount);
			void Render(ElementRendererData& rendererData, CommandBufferBuilder& commandBuffer, const Pointer<const RenderElement>* elements, std::size_t elementCount) override;
			void Reset(ElementRendererData& rendererData, RenderFrame& currentFrame);

		private:
			std::shared_ptr<AbstractBuffer> m_indexBuffer;
			std::size_t m_maxVertexBufferSize;
			std::size_t m_maxVertexCount;
			RenderDevice& m_device;
	};

	struct SpriteChainRendererData : public ElementRendererData
	{
		struct DrawCall
		{
			const AbstractBuffer* vertexBuffer;
			const RenderPipeline* renderPipeline;
			const ShaderBinding* instanceBinding;
			const ShaderBinding* materialBinding;
			std::size_t firstIndex;
			std::size_t quadCount;
		};

		struct DrawCallIndices
		{
			std::size_t start;
			std::size_t count;
		};

		std::unordered_map<const RenderSpriteChain*, DrawCallIndices> drawCallPerElement;
		std::vector<DrawCall> drawCalls;
		std::vector<std::shared_ptr<AbstractBuffer>> vertexBuffers;
	};
}

#include <Nazara/Graphics/SpriteChainRenderer.inl>

#endif
