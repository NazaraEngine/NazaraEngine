// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/SpriteChainRenderer.hpp>
#include <Nazara/Graphics/SubmeshRenderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	FramePipeline::FramePipeline() :
	m_elementRenderers(BasicRenderElementCount),
	m_debugDrawer(*Graphics::Instance()->GetRenderDevice())
	{
		m_elementRenderers[UnderlyingCast(BasicRenderElement::SpriteChain)] = std::make_unique<SpriteChainRenderer>(*Graphics::Instance()->GetRenderDevice());
		m_elementRenderers[UnderlyingCast(BasicRenderElement::Submesh)] = std::make_unique<SubmeshRenderer>();
	}

	FramePipeline::~FramePipeline() = default;
}
