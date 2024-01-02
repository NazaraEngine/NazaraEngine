// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	FramePipeline::FramePipeline() :
	m_debugDrawer(*Graphics::Instance()->GetRenderDevice())
	{
	}

	FramePipeline::~FramePipeline() = default;
}
