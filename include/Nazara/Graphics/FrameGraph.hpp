// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FRAMEGRAPH_HPP
#define NAZARA_FRAMEGRAPH_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/FramePass.hpp>
#include <Nazara/Graphics/FramePassAttachment.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RenderPass.hpp>
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace Nz
{
	class NAZARA_GRAPHICS_API FrameGraph
	{
		public:
			FrameGraph() = default;
			FrameGraph(const FrameGraph&) = delete;
			FrameGraph(FrameGraph&&) noexcept = default;
			~FrameGraph() = default;

			inline std::size_t AddAttachment(FramePassAttachment attachment);
			inline FramePass& AddPass(std::string name);

			BakedFrameGraph Bake();

			inline void SetBackbufferOutput(std::size_t backbufferOutput);

			FrameGraph& operator=(const FrameGraph&) = delete;
			FrameGraph& operator=(FrameGraph&&) noexcept = default;

		private:
			struct PassBarriers;

			using BarrierList = std::vector<PassBarriers>;
			using PassList = std::vector<std::size_t /*PassIndex*/>;
			using AttachmentIdToPassMap = std::unordered_map<std::size_t /*resourceIndex*/, PassList /*passIndexes*/>;
			using AttachmentIdToTextureId = std::unordered_map<std::size_t /*attachmentId*/, std::size_t /*textureId*/>;
			using PassIdToPhysicalPassIndex = std::unordered_map<std::size_t /*passId*/, std::size_t /*physicalPassId*/>;
			using TextureTransition = BakedFrameGraph::TextureTransition;

			struct Barrier
			{
				std::size_t textureId;
				MemoryAccessFlags access;
				PipelineStageFlags stages;
				TextureLayout layout;
			};

			struct PassBarriers
			{
				std::vector<Barrier> invalidationBarriers;
				std::vector<Barrier> flushBarriers;
			};

			struct PhysicalPassData
			{
				struct Subpass
				{
					std::size_t passIndex;
				};

				std::string name;
				std::vector<TextureTransition> textureTransitions;
				std::vector<Subpass> passes;
			};

			struct TextureData
			{
				PixelFormat format;
				TextureUsageFlags usage;
				unsigned int width;
				unsigned int height;
			};

			struct WorkData
			{
				std::size_t backbufferResourceIndex;
				std::vector<std::shared_ptr<RenderPass>> renderPasses;
				std::vector<PhysicalPassData> physicalPasses;
				std::vector<TextureData> textures;
				AttachmentIdToPassMap attachmentReadList;
				AttachmentIdToPassMap attachmentWriteList;
				AttachmentIdToTextureId attachmentToTextures;
				BarrierList barrierList;
				PassList passList;
				PassIdToPhysicalPassIndex passIdToPhysicalPassIndex;
			};

			void AssignPhysicalPasses();
			void AssignPhysicalTextures();
			void BuildBarriers();
			void BuildPhysicalBarriers();
			void BuildPhysicalPassDependencies(std::size_t colorAttachmentCount, bool hasDepthStencilAttachment, std::vector<RenderPass::Attachment>& renderPassAttachments, std::vector<RenderPass::SubpassDescription>& subpasses, std::vector<RenderPass::SubpassDependency>& dependencies);
			void BuildPhysicalPasses();
			void BuildReadWriteList();
			void RemoveDuplicatePasses();
			void ReorderPasses();
			void TraverseGraph(std::size_t passIndex);

			std::optional<std::size_t> m_backbufferOutput;
			std::vector<FramePass> m_framePasses;
			std::vector<FramePassAttachment> m_attachments;
			WorkData m_pending;
	};
}

#include <Nazara/Graphics/FrameGraph.inl>

#endif
