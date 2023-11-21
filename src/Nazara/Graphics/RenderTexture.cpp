// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderTexture.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	RenderTexture::RenderTexture(std::shared_ptr<Texture> texture) :
	m_targetTexture(std::move(texture)),
	m_textureSize(Vector2ui(m_targetTexture->GetSize()))
	{
	}

	void RenderTexture::OnBuildGraph(FrameGraph& graph, std::size_t attachmentIndex) const
	{
		graph.BindExternalTexture(attachmentIndex, m_targetTexture);
	}

	void RenderTexture::OnRenderEnd(RenderResources& /*renderFrame*/, const BakedFrameGraph& /*frameGraph*/, std::size_t /*finalAttachment*/) const
	{
	}

	const Vector2ui& RenderTexture::GetSize() const
	{
		return m_textureSize;
	}
}
