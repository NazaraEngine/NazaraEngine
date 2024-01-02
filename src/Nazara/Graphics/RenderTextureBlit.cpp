// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderTextureBlit.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderResources.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	std::size_t RenderTextureBlit::OnBuildGraph(FrameGraph& graph, std::size_t attachmentIndex) const
	{
		std::size_t linkAttachment = graph.AddDummyAttachment();

		FramePass& blitPass = graph.AddPass("Blit to texture");
		blitPass.AddInput(attachmentIndex);
		blitPass.SetInputAccess(0, TextureLayout::TransferSource, PipelineStage::Transfer, MemoryAccess::MemoryRead);
		blitPass.SetInputUsage(0, TextureUsage::TransferSource);

		blitPass.AddOutput(linkAttachment);

		blitPass.SetCommandCallback([this, attachmentIndex](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			const std::shared_ptr<Texture>& sourceTexture = env.frameGraph.GetAttachmentTexture(attachmentIndex);

			Vector2ui sourceTextureSize = Vector2ui(sourceTexture->GetSize());
			Vector2ui targetTextureSize = Vector2ui(m_targetTexture->GetSize());

			Boxui fromBox(0, 0, 0, sourceTextureSize.x, sourceTextureSize.y, 1);
			Boxui toBox(0, 0, 0, targetTextureSize.x, targetTextureSize.y, 1);

			builder.TextureBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {}, MemoryAccess::TransferWrite, TextureLayout::Undefined, TextureLayout::TransferDestination, *m_targetTexture);
			builder.BlitTexture(*sourceTexture, fromBox, TextureLayout::TransferSource, *m_targetTexture, toBox, TextureLayout::TransferDestination, m_samplerFilter);
			builder.TextureBarrier(PipelineStage::Transfer, m_targetPipelineStage, MemoryAccess::TransferWrite, m_targetMemoryFlags, TextureLayout::TransferDestination, m_targetLayout, *m_targetTexture);
		});

		return linkAttachment;
	}

	const Vector2ui& RenderTextureBlit::GetSize() const
	{
		return m_textureSize;
	}
}
