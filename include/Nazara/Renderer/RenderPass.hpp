// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERPASS_HPP
#define NAZARA_RENDERER_RENDERPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/PixelFormat.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <limits>
#include <optional>
#include <vector>

namespace Nz
{
	class NAZARA_RENDERER_API RenderPass
	{
		public:
			struct Attachment;
			struct SubpassDependency;
			struct SubpassDescription;

			inline RenderPass(std::vector<Attachment> attachments, std::vector<SubpassDescription> subpassDescriptions, std::vector<SubpassDependency> subpassDependencies);
			RenderPass(const RenderPass&) = delete;
			RenderPass(RenderPass&&) noexcept = default;
			virtual ~RenderPass();

			inline const Attachment& GetAttachment(std::size_t attachmentIndex) const;
			inline std::size_t GetAttachmentCount() const;
			inline const std::vector<Attachment>& GetAttachments() const;
			inline const std::vector<SubpassDescription>& GetSubpassDescriptions() const;
			inline const std::vector<SubpassDependency>& GetSubpassDependencies() const;

			virtual void UpdateDebugName(std::string_view name) = 0;

			RenderPass& operator=(const RenderPass&) = delete;
			RenderPass& operator=(RenderPass&&) noexcept = default;

			struct Attachment
			{
				PixelFormat format;
				AttachmentLoadOp loadOp = AttachmentLoadOp::Load;
				AttachmentLoadOp stencilLoadOp = AttachmentLoadOp::Load;
				AttachmentStoreOp storeOp = AttachmentStoreOp::Store;
				AttachmentStoreOp stencilStoreOp = AttachmentStoreOp::Store;
				TextureLayout initialLayout = TextureLayout::Undefined;
				TextureLayout finalLayout = TextureLayout::Present;
			};

			struct AttachmentReference
			{
				std::size_t attachmentIndex;
				TextureLayout attachmentLayout = TextureLayout::ColorInput;
			};

			struct SubpassDependency
			{
				std::size_t fromSubpassIndex;
				PipelineStageFlags fromStages;
				MemoryAccessFlags fromAccessFlags;

				std::size_t toSubpassIndex;
				PipelineStageFlags toStages;
				MemoryAccessFlags toAccessFlags;
				bool tilable = false;
			};

			struct SubpassDescription
			{
				std::vector<AttachmentReference> colorAttachment;
				std::vector<AttachmentReference> inputAttachments;
				std::vector<std::size_t> preserveAttachments;
				std::optional<AttachmentReference> depthStencilAttachment;
			};

			static constexpr std::size_t ExternalSubpassIndex = std::numeric_limits<std::size_t>::max();

		protected:
			std::vector<Attachment> m_attachments;
			std::vector<SubpassDependency> m_subpassDependencies;
			std::vector<SubpassDescription> m_subpassDescriptions;
	};
}

#include <Nazara/Renderer/RenderPass.inl>

#endif // NAZARA_RENDERER_RENDERPASS_HPP
