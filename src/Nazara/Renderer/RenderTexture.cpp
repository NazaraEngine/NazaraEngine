// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	RenderTexture::RenderTexture(std::shared_ptr<Texture> texture, PipelineStage targetPipelineStage, MemoryAccessFlags targetMemoryFlags, TextureLayout targetLayout) :
	m_targetTexture(std::move(texture)),
	m_targetMemoryFlags(targetMemoryFlags),
	m_targetPipelineStage(targetPipelineStage),
	m_targetLayout(targetLayout),
	m_textureSize(Vector2ui(m_targetTexture->GetSize()))
	{
	}

	void RenderTexture::BlitTexture(RenderFrame& /*renderFrame*/, CommandBufferBuilder& builder, const Texture& texture) const
	{
		Vector3ui textureSize = texture.GetSize();
		Vector3ui targetTextureSize = m_targetTexture->GetSize();

		builder.BeginDebugRegion("Blit to texture", Color::Blue());
		{
			builder.TextureBarrier(PipelineStage::ColorOutput, PipelineStage::Transfer, MemoryAccess::ColorWrite, MemoryAccess::TransferRead, TextureLayout::ColorOutput, TextureLayout::TransferSource, texture);
			builder.TextureBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {}, MemoryAccess::TransferWrite, TextureLayout::Undefined, TextureLayout::TransferDestination, *m_targetTexture);

			Boxui fromBox(0, 0, 0, textureSize.x, textureSize.y, 1);
			Boxui toBox(0, 0, 0, targetTextureSize.x, targetTextureSize.y, 1);

			builder.BlitTexture(texture, fromBox, TextureLayout::TransferSource, *m_targetTexture, toBox, TextureLayout::TransferDestination, SamplerFilter::Linear);

			builder.TextureBarrier(PipelineStage::Transfer, m_targetPipelineStage, MemoryAccess::TransferWrite, m_targetMemoryFlags, TextureLayout::TransferDestination, m_targetLayout, *m_targetTexture);
		}
		builder.EndDebugRegion();
	}

	const Vector2ui& RenderTexture::GetSize() const
	{
		return m_textureSize;
	}
}
