// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
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
	void RenderTextureBlit::OnBuildGraph(FrameGraph& graph, std::size_t attachmentIndex) const
	{
		graph.AddOutput(attachmentIndex);
	}

	void RenderTextureBlit::OnRenderEnd(RenderResources& resources, const BakedFrameGraph& frameGraph, std::size_t finalAttachment) const
	{
		const std::shared_ptr<Texture>& sourceTexture = frameGraph.GetAttachmentTexture(finalAttachment);

		Vector2ui sourceTextureSize = Vector2ui(sourceTexture->GetSize());
		Vector2ui targetTextureSize = Vector2ui(m_targetTexture->GetSize());

		resources.Execute([&](CommandBufferBuilder& builder)
		{
			builder.BeginDebugRegion("Blit to texture", Color::Blue());
			{
				builder.TextureBarrier(PipelineStage::ColorOutput, PipelineStage::Transfer, MemoryAccess::ColorWrite, MemoryAccess::TransferRead, TextureLayout::ColorOutput, TextureLayout::TransferSource, *sourceTexture);
				builder.TextureBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {}, MemoryAccess::TransferWrite, TextureLayout::Undefined, TextureLayout::TransferDestination, *m_targetTexture);

				Boxui fromBox(0, 0, 0, sourceTextureSize.x, sourceTextureSize.y, 1);
				Boxui toBox(0, 0, 0, targetTextureSize.x, targetTextureSize.y, 1);

				builder.BlitTexture(*sourceTexture, fromBox, TextureLayout::TransferSource, *m_targetTexture, toBox, TextureLayout::TransferDestination, m_samplerFilter);
				builder.TextureBarrier(PipelineStage::Transfer, m_targetPipelineStage, MemoryAccess::TransferWrite, m_targetMemoryFlags, TextureLayout::TransferDestination, m_targetLayout, *m_targetTexture);
			}
			builder.EndDebugRegion();
		}, QueueType::Graphics);
	}

	const Vector2ui& RenderTextureBlit::GetSize() const
	{
		return m_textureSize;
	}
}
