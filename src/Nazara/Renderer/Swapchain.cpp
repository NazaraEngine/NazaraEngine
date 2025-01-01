// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Renderer/Swapchain.hpp>
#include <Nazara/Renderer/Texture.hpp>

namespace Nz
{
	Swapchain::~Swapchain() = default;

	void Swapchain::BuildRenderPass(PixelFormat colorFormat, PixelFormat depthFormat, std::vector<RenderPass::Attachment>& attachments, std::vector<RenderPass::SubpassDescription>& subpassDescriptions, std::vector<RenderPass::SubpassDependency>& subpassDependencies)
	{
		assert(colorFormat != PixelFormat::Undefined);

		attachments.push_back({
			colorFormat,
			AttachmentLoadOp::Clear,
			AttachmentLoadOp::Discard,
			AttachmentStoreOp::Store,
			AttachmentStoreOp::Discard,
			TextureLayout::Undefined,
			TextureLayout::Present
		});

		RenderPass::AttachmentReference colorReference = {
			0,
			TextureLayout::ColorOutput
		};

		subpassDescriptions.push_back(
		{
			{ colorReference },
			{},
			{},
			std::nullopt
		});

		subpassDependencies.push_back({
			RenderPass::ExternalSubpassIndex,
			PipelineStage::ColorOutput,
			{},

			0,
			PipelineStage::ColorOutput,
			MemoryAccess::ColorWrite,

			true //< tilable
		});

		if (depthFormat != PixelFormat::Undefined)
		{
			attachments.push_back({
				depthFormat,
				AttachmentLoadOp::Clear,
				AttachmentLoadOp::Discard,
				AttachmentStoreOp::Discard,
				AttachmentStoreOp::Discard,
				TextureLayout::Undefined,
				TextureLayout::DepthStencilReadWrite
			});

			subpassDescriptions.front().depthStencilAttachment = RenderPass::AttachmentReference{
				1,
				TextureLayout::DepthStencilReadWrite
			};

			auto& subpassDependency = subpassDependencies.front();
			subpassDependency.fromStages |= PipelineStage::FragmentTestsEarly;
			subpassDependency.toStages |= PipelineStage::FragmentTestsEarly;
			subpassDependency.toAccessFlags |= MemoryAccess::DepthStencilWrite;
		}
	}
}
