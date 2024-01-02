// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
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

	std::size_t RenderTexture::OnBuildGraph(FrameGraph& graph, std::size_t attachmentIndex) const
	{
		graph.BindExternalTexture(attachmentIndex, m_targetTexture);
		return attachmentIndex;
	}

	const Vector2ui& RenderTexture::GetSize() const
	{
		return m_textureSize;
	}
}
