// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FRAMEGRAPH_HPP
#define NAZARA_GRAPHICS_FRAMEGRAPH_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/FrameGraphStructs.hpp>
#include <Nazara/Graphics/FramePass.hpp>
#include <Nazara/Graphics/FramePassAttachment.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RenderPass.hpp>
#include <limits>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace Nz
{
	class NAZARA_GRAPHICS_API FrameGraph
	{
		friend class BakedFrameGraph;

		public:
			FrameGraph() = default;
			FrameGraph(const FrameGraph&) = delete;
			FrameGraph(FrameGraph&&) noexcept = default;
			~FrameGraph() = default;

			inline std::size_t AddAttachment(FramePassAttachment attachment);
			inline std::size_t AddAttachmentArray(FramePassAttachment attachment, unsigned int layerCount);
			inline std::size_t AddAttachmentArrayLayer(std::size_t attachmentId, std::size_t layerIndex);
			inline std::size_t AddAttachmentCube(FramePassAttachment attachment);
			inline std::size_t AddAttachmentCubeFace(std::size_t attachmentId, CubemapFace face);
			inline std::size_t AddAttachmentProxy(std::string name, std::size_t attachmentId);
			inline std::size_t AddDummyAttachment();
			inline FramePass& AddPass(std::string name);
			inline void AddOutput(std::size_t attachmentIndex);

			BakedFrameGraph Bake();

			inline void BindExternalTexture(std::size_t attachmentIndex, std::shared_ptr<Texture> texture);

			FrameGraph& operator=(const FrameGraph&) = delete;
			FrameGraph& operator=(FrameGraph&&) noexcept = default;

		private:
			struct PassBarriers;

			using BarrierList = std::vector<PassBarriers>;
			using PassList = std::vector<std::size_t /*PassIndex*/>;
			using AttachmentIdToPassMap = std::unordered_map<std::size_t /*resourceIndex*/, PassList /*passIndexes*/>;
			using AttachmentIdToPassId = std::unordered_map<std::size_t /*attachmentId*/, std::size_t /*passId*/>;
			using AttachmentIdToTextureId = std::unordered_map<std::size_t /*attachmentId*/, std::size_t /*textureId*/>;
			using PassIdToPhysicalPassIndex = std::unordered_map<std::size_t /*passId*/, std::size_t /*physicalPassId*/>;
			using TextureBarrier = BakedFrameGraph::TextureBarrier;

			struct AttachmentArray : FramePassAttachment
			{
				unsigned int layerCount;
			};

			struct AttachmentCube : FramePassAttachment
			{
			};

			struct AttachmentLayer
			{
				std::size_t attachmentId;
				std::size_t layerIndex;
			};

			struct AttachmentProxy
			{
				std::size_t attachmentId;
				std::string name;
			};

			struct Barrier
			{
				std::size_t textureId;
				MemoryAccessFlags access;
				PipelineStageFlags stages;
				TextureLayout layout;
			};

			struct DummyAttachment
			{
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
				std::vector<TextureBarrier> textureBarrier;
				std::vector<Subpass> passes;
			};

			struct WorkData
			{
				std::vector<std::shared_ptr<RenderPass>> renderPasses;
				std::vector<PhysicalPassData> physicalPasses;
				std::vector<FrameGraphTextureData> textures;
				std::vector<std::size_t> texture2DPool;
				std::vector<std::size_t> texture2DArrayPool;
				std::vector<std::size_t> textureCubePool;
				AttachmentIdToPassId attachmentLastUse;
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
			bool HasAttachment(const std::vector<FramePass::Input>& inputs, std::size_t attachmentIndex) const;
			void RemoveDuplicatePasses();
			std::size_t ResolveAttachmentIndex(std::size_t attachmentIndex) const;
			void RegisterPassInput(std::size_t passIndex, std::size_t attachmentIndex);
			std::size_t RegisterTexture(std::size_t attachmentIndex);
			void ReorderPasses();
			void TraverseGraph(std::size_t passIndex);

			using AttachmentType = std::variant<FramePassAttachment, AttachmentProxy, AttachmentArray, AttachmentCube, AttachmentLayer, DummyAttachment>;

			static constexpr std::size_t InvalidAttachmentIndex = std::numeric_limits<std::size_t>::max();
			static constexpr std::size_t InvalidTextureIndex = std::numeric_limits<std::size_t>::max();

			std::vector<std::size_t> m_graphOutputs;
			std::vector<FramePass> m_framePasses;
			std::vector<AttachmentType> m_attachments;
			std::unordered_map<std::size_t, std::shared_ptr<Texture>> m_externalTextures;
			WorkData m_pending;
	};
}

#include <Nazara/Graphics/FrameGraph.inl>

#endif // NAZARA_GRAPHICS_FRAMEGRAPH_HPP
