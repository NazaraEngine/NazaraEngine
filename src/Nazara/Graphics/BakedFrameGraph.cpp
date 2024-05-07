// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>

namespace Nz
{
	BakedFrameGraph::BakedFrameGraph(std::vector<PassData> passes, std::vector<TextureData> textures, AttachmentIdToTextureId attachmentIdToTextureMapping, PassIdToPhysicalPassIndex passIdToPhysicalPassMapping) :
	m_passes(std::move(passes)),
	m_textures(std::move(textures)),
	m_attachmentToTextureMapping(std::move(attachmentIdToTextureMapping)),
	m_passIdToPhysicalPassMapping(std::move(passIdToPhysicalPassMapping))
	{
		const std::shared_ptr<RenderDevice>& renderDevice = Graphics::Instance()->GetRenderDevice();
		m_commandPool = renderDevice->InstantiateCommandPool(QueueType::Graphics);
	}

	void BakedFrameGraph::Execute(RenderResources& renderResources)
	{
		for (auto& passData : m_passes)
		{
			bool regenerateCommandBuffer = (passData.forceCommandBufferRegeneration || passData.commandBuffer == nullptr);
			if (passData.executionCallback)
			{
				switch (passData.executionCallback())
				{
					case FramePassExecution::Execute:
						break;

					case FramePassExecution::Skip:
						if (passData.commandBuffer)
						{
							renderResources.PushForRelease(std::move(passData.commandBuffer));
							passData.commandBuffer.reset();
						}
						continue; //< Skip the pass

					case FramePassExecution::UpdateAndExecute:
						regenerateCommandBuffer = true;
						break;
				}
			}

			if (!regenerateCommandBuffer)
				continue;

			if (passData.commandBuffer)
				renderResources.PushForRelease(std::move(passData.commandBuffer));

			passData.commandBuffer = m_commandPool->BuildCommandBuffer([&](CommandBufferBuilder& builder)
			{
				for (auto& textureTransition : passData.invalidationBarriers)
				{
					const std::shared_ptr<Texture>& texture = m_textures[textureTransition.textureId].texture;
					builder.TextureBarrier(textureTransition.srcStageMask, textureTransition.dstStageMask, textureTransition.srcAccessMask, textureTransition.dstAccessMask, textureTransition.oldLayout, textureTransition.newLayout, *texture);
				}

				if (passData.framebuffer)
					builder.BeginRenderPass(*passData.framebuffer, *passData.renderPass, passData.renderRect, passData.outputClearValues.data(), passData.outputClearValues.size());

				if (!passData.name.empty())
					builder.BeginDebugRegion(passData.name, Color::Green());

				FramePassEnvironment env{
					.frameGraph = *this,
					.renderResources = renderResources,
					.renderRect = passData.renderRect
				};

				bool first = true;
				for (auto& subpass : passData.subpasses)
				{
					if (!first)
						builder.NextSubpass();

					first = false;

					subpass.commandCallback(builder, env);
				}

				if (!passData.name.empty())
					builder.EndDebugRegion();

				if (passData.framebuffer)
					builder.EndRenderPass();
			});

			passData.forceCommandBufferRegeneration = false;
		}

		//TODO: Submit all commands buffer at once
		for (auto& passData : m_passes)
		{
			if (passData.commandBuffer)
				renderResources.SubmitCommandBuffer(passData.commandBuffer.get(), QueueType::Graphics);
		}
	}

	const std::shared_ptr<Texture>& BakedFrameGraph::GetAttachmentTexture(std::size_t attachmentIndex) const
	{
		auto it = m_attachmentToTextureMapping.find(attachmentIndex);
		if (it == m_attachmentToTextureMapping.end() || it->second == FrameGraph::InvalidTextureIndex)
		{
			static std::shared_ptr<Texture> dummy;
			return dummy;
		}

		std::size_t textureIndex = it->second;
		assert(textureIndex < m_textures.size());
		return m_textures[textureIndex].texture;
	}

	const std::shared_ptr<RenderPass>& BakedFrameGraph::GetRenderPass(std::size_t passIndex) const
	{
		auto it = m_attachmentToTextureMapping.find(passIndex);
		if (it == m_attachmentToTextureMapping.end() || it->second == FrameGraph::InvalidTextureIndex)
		{
			static std::shared_ptr<RenderPass> dummy;
			return dummy;
		}

		std::size_t physicalPassIndex = it->second;
		assert(physicalPassIndex < m_passes.size());
		return m_passes[physicalPassIndex].renderPass;
	}

	bool BakedFrameGraph::Resize(RenderResources& renderResources, std::span<Vector2ui> viewerTargetSizes)
	{
		if (std::equal(m_viewerSizes.begin(), m_viewerSizes.end(), viewerTargetSizes.begin(), viewerTargetSizes.end()))
			return false;

		const std::shared_ptr<RenderDevice>& renderDevice = Graphics::Instance()->GetRenderDevice();

		auto ComputeTextureSize = [&](TextureData& textureData) -> Vector2ui32
		{
			Vector2ui32 texDimensions(1, 1);
			switch (textureData.size)
			{
				case FramePassAttachmentSize::Fixed:
					texDimensions.x = textureData.width;
					texDimensions.y = textureData.height;
					break;

				case FramePassAttachmentSize::ViewerTargetFactor:
					texDimensions = viewerTargetSizes[textureData.viewerIndex];
					texDimensions.x *= textureData.width;
					texDimensions.y *= textureData.height;
					texDimensions /= 100'000;
					break;
			}

			return texDimensions;
		};

		// Delete previous textures to make some room in VRAM
		for (auto& passData : m_passes)
		{
			if (passData.commandBuffer)
				renderResources.PushForRelease(std::move(passData.commandBuffer));

			if (passData.framebuffer)
				renderResources.PushForRelease(std::move(passData.framebuffer));
		}

		for (auto& textureData : m_textures)
		{
			if (!textureData.texture)
				continue;

			// Check if texture dimension changed
			Vector3ui32 curSize = textureData.texture->GetSize();
			auto [newWidth, newHeight] = ComputeTextureSize(textureData);
			if (newWidth == curSize.x && newHeight == curSize.y)
				continue;

			// Dimensions changed, recreated texture
			renderResources.PushForRelease(std::move(textureData.texture));
		}

		for (auto& textureData : m_textures)
		{
			if (textureData.texture)
				continue;

			if (textureData.viewData)
			{
				TextureData& parentTexture = m_textures[textureData.viewData->parentTextureId];

				// This is a view on another texture
				TextureViewInfo textureViewParams;
				textureViewParams.viewType = textureData.type;
				textureViewParams.reinterpretFormat = textureData.format;
				textureViewParams.baseArrayLayer = SafeCast<unsigned int>(textureData.viewData->arrayLayer);

				textureData.texture = parentTexture.texture->CreateView(textureViewParams);
			}
			else
			{
				TextureInfo textureCreationParams;
				textureCreationParams.type = textureData.type;
				textureCreationParams.usageFlags = textureData.usage;
				textureCreationParams.pixelFormat = textureData.format;
				textureCreationParams.levelCount = 1;

				textureCreationParams.layerCount = textureData.layerCount;
				if (textureCreationParams.type == ImageType::Cubemap)
					textureCreationParams.layerCount *= 6;

				auto [width, height] = ComputeTextureSize(textureData);
				textureCreationParams.width = width;
				textureCreationParams.height = height;

				textureData.texture = renderDevice->InstantiateTexture(textureCreationParams);
				if (!textureData.name.empty())
					textureData.texture->UpdateDebugName(textureData.name);
			}
		}

		std::vector<std::shared_ptr<Texture>> textures;
		for (auto& passData : m_passes)
		{
			textures.clear();

			if (!passData.outputTextureIndices.empty())
			{
				unsigned int framebufferWidth = std::numeric_limits<unsigned int>::max();
				unsigned int framebufferHeight = std::numeric_limits<unsigned int>::max();
				for (std::size_t textureId : passData.outputTextureIndices)
				{
					auto& textureData = m_textures[textureId];
					textures.push_back(textureData.texture);

					auto [width, height] = ComputeTextureSize(textureData);

					framebufferWidth = std::min(framebufferWidth, width);
					framebufferHeight = std::min(framebufferHeight, height);
				}

				passData.renderRect = Recti(0, 0, int(framebufferWidth), int(framebufferHeight));

				passData.framebuffer = renderDevice->InstantiateFramebuffer(framebufferWidth, framebufferHeight, passData.renderPass, textures);
				if (!passData.name.empty())
					passData.framebuffer->UpdateDebugName(passData.name);
			}
			else
				passData.renderRect = Recti(0, 0, -1, -1);

			passData.forceCommandBufferRegeneration = true;
		}

		m_viewerSizes.assign(viewerTargetSizes.begin(), viewerTargetSizes.end());
		return true;
	}
}
