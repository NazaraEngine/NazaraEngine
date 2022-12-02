// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_BAKEDFRAMEGRAPH_HPP
#define NAZARA_GRAPHICS_BAKEDFRAMEGRAPH_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/FrameGraphStructs.hpp>
#include <Nazara/Graphics/FramePass.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/CommandPool.hpp>
#include <Nazara/Renderer/Framebuffer.hpp>
#include <Nazara/Renderer/RenderPass.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <vector>

namespace Nz
{
	class RenderFrame;

	class NAZARA_GRAPHICS_API BakedFrameGraph
	{
		friend class FrameGraph;

		public:
			BakedFrameGraph() = default;
			BakedFrameGraph(const BakedFrameGraph&) = delete;
			BakedFrameGraph(BakedFrameGraph&&) noexcept = default;
			~BakedFrameGraph() = default;

			void Execute(RenderFrame& renderFrame);

			const std::shared_ptr<Texture>& GetAttachmentTexture(std::size_t attachmentIndex) const;
			const std::shared_ptr<RenderPass>& GetRenderPass(std::size_t passIndex) const;

			bool Resize(RenderFrame& renderFrame);

			BakedFrameGraph& operator=(const BakedFrameGraph&) = delete;
			BakedFrameGraph& operator=(BakedFrameGraph&&) noexcept = default;

		private:
			struct PassData;
			struct TextureData;
			using AttachmentIdToTextureId = std::unordered_map<std::size_t /*attachmentId*/, std::size_t /*textureId*/>;
			using PassIdToPhysicalPassIndex = std::unordered_map<std::size_t /*passId*/, std::size_t /*physicalPassId*/>;

			BakedFrameGraph(std::vector<PassData> passes, std::vector<TextureData> textures, AttachmentIdToTextureId attachmentIdToTextureMapping, PassIdToPhysicalPassIndex passIdToPhysicalPassMapping);

			struct TextureBarrier
			{
				std::size_t textureId;
				MemoryAccessFlags dstAccessMask;
				MemoryAccessFlags srcAccessMask;
				PipelineStageFlags dstStageMask;
				PipelineStageFlags srcStageMask;
				TextureLayout newLayout;
				TextureLayout oldLayout;
			};

			struct SubpassData
			{
				FramePass::CommandCallback commandCallback;
			};

			struct PassData
			{
				CommandBufferPtr commandBuffer;
				std::shared_ptr<Framebuffer> framebuffer;
				std::shared_ptr<RenderPass> renderPass;
				std::string name;
				std::vector<std::size_t> outputTextureIndices;
				std::vector<CommandBufferBuilder::ClearValues> outputClearValues;
				std::vector<SubpassData> subpasses;
				std::vector<TextureBarrier> invalidationBarriers;
				FramePass::ExecutionCallback executionCallback;
				Recti renderRect;
				bool forceCommandBufferRegeneration = true;
			};

			struct TextureData : FrameGraphTextureData
			{
				std::shared_ptr<Texture> texture;
			};

			std::shared_ptr<CommandPool> m_commandPool;
			std::vector<PassData> m_passes;
			std::vector<TextureData> m_textures;
			AttachmentIdToTextureId m_attachmentToTextureMapping;
			PassIdToPhysicalPassIndex m_passIdToPhysicalPassMapping;
			unsigned int m_height;
			unsigned int m_width;
	};
}

#include <Nazara/Graphics/BakedFrameGraph.inl>

#endif // NAZARA_GRAPHICS_BAKEDFRAMEGRAPH_HPP
