// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	BakedFrameGraph::BakedFrameGraph(std::vector<PassData> passes, std::vector<TextureData> textures, AttachmentIdToTextureId attachmentIdToTextureMapping, PassIdToPhysicalPassIndex passIdToPhysicalPassMapping) :
	m_passes(std::move(passes)),
	m_textures(std::move(textures)),
	m_attachmentToTextureMapping(std::move(attachmentIdToTextureMapping)),
	m_passIdToPhysicalPassMapping(std::move(passIdToPhysicalPassMapping)),
	m_height(0),
	m_width(0)
	{
		const std::shared_ptr<RenderDevice>& renderDevice = Graphics::Instance()->GetRenderDevice();
		m_commandPool = renderDevice->InstantiateCommandPool(QueueType::Graphics);
	}

	void BakedFrameGraph::Execute(RenderFrame& renderFrame)
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
							renderFrame.PushForRelease(std::move(passData.commandBuffer));
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
				renderFrame.PushForRelease(std::move(passData.commandBuffer));

			passData.commandBuffer = m_commandPool->BuildCommandBuffer([&](CommandBufferBuilder& builder)
			{
				for (auto& textureTransition : passData.invalidationBarriers)
				{
					const std::shared_ptr<Texture>& texture = m_textures[textureTransition.textureId].texture;
					builder.TextureBarrier(textureTransition.srcStageMask, textureTransition.dstStageMask, textureTransition.srcAccessMask, textureTransition.dstAccessMask, textureTransition.oldLayout, textureTransition.newLayout, *texture);
				}

				builder.BeginRenderPass(*passData.framebuffer, *passData.renderPass, passData.renderRect, passData.outputClearValues.data(), passData.outputClearValues.size());

				if (!passData.name.empty())
					builder.BeginDebugRegion(passData.name, Color::Green);

				FramePassEnvironment env{
					*this,
					passData.renderRect,
					renderFrame
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

				builder.EndRenderPass();
			});

			passData.forceCommandBufferRegeneration = false;
		}

		//TODO: Submit all commands buffer at once
		for (auto& passData : m_passes)
		{
			if (passData.commandBuffer)
				renderFrame.SubmitCommandBuffer(passData.commandBuffer.get(), QueueType::Graphics);
		}
	}

	const std::shared_ptr<Texture>& BakedFrameGraph::GetAttachmentTexture(std::size_t attachmentIndex) const
	{
		auto it = m_attachmentToTextureMapping.find(attachmentIndex);
		if (it == m_attachmentToTextureMapping.end())
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
		if (it == m_attachmentToTextureMapping.end())
		{
			static std::shared_ptr<RenderPass> dummy;
			return dummy;
		}

		std::size_t physicalPassIndex = it->second;
		assert(physicalPassIndex < m_passes.size());
		return m_passes[physicalPassIndex].renderPass;
	}

	bool BakedFrameGraph::Resize(RenderFrame& renderFrame)
	{
		auto [frameWidth, frameHeight] = renderFrame.GetSize();
		if (m_width == frameWidth && m_height == frameHeight)
			return false;

		const std::shared_ptr<RenderDevice>& renderDevice = Graphics::Instance()->GetRenderDevice();

		// Delete previous textures to make some room in VRAM
		for (auto& passData : m_passes)
		{
			renderFrame.PushForRelease(std::move(passData.commandBuffer));
			renderFrame.PushForRelease(std::move(passData.framebuffer));
		}

		for (auto& textureData : m_textures)
			renderFrame.PushForRelease(std::move(textureData.texture));

		for (auto& textureData : m_textures)
		{
			TextureInfo textureCreationParams;
			textureCreationParams.type = ImageType::E2D;
			textureCreationParams.usageFlags = textureData.usage;
			textureCreationParams.pixelFormat = textureData.format;
			if (textureData.hasFixedSize)
			{
				textureCreationParams.width = textureData.width;
				textureCreationParams.height = textureData.height;
			}
			else
			{
				textureCreationParams.width = textureData.width * frameWidth / 100'000;
				textureCreationParams.height = textureData.height * frameHeight / 100'000;
			}

			textureData.texture = renderDevice->InstantiateTexture(textureCreationParams);
			if (!textureData.name.empty())
				textureData.texture->UpdateDebugName(textureData.name);
		}

		std::vector<std::shared_ptr<Texture>> textures;
		for (auto& passData : m_passes)
		{
			textures.clear();

			unsigned int framebufferWidth = std::numeric_limits<unsigned int>::max();
			unsigned int framebufferHeight = std::numeric_limits<unsigned int>::max();
			for (std::size_t textureId : passData.outputTextureIndices)
			{
				auto& textureData = m_textures[textureId];
				textures.push_back(textureData.texture);

				unsigned int width;
				unsigned int height;
				if (textureData.hasFixedSize)
				{
					width = textureData.width;
					height = textureData.height;
				}
				else
				{
					width = frameWidth * textureData.width / 100'000;
					height = frameHeight * textureData.height / 100'000;
				}

				framebufferWidth = std::min(framebufferWidth, width);
				framebufferHeight = std::min(framebufferHeight, height);
			}

			passData.renderRect.Set(0, 0, int(framebufferWidth), int(framebufferHeight));

			passData.framebuffer = renderDevice->InstantiateFramebuffer(framebufferWidth, framebufferHeight, passData.renderPass, textures);
			if (!passData.name.empty())
				passData.framebuffer->UpdateDebugName(passData.name);

			passData.forceCommandBufferRegeneration = true;
		}

		m_width = frameWidth;
		m_height = frameHeight;

		return true;
	}
}
