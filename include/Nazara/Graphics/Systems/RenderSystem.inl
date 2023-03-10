// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <type_traits>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline FramePipeline& RenderSystem::GetFramePipeline()
	{
		return *m_pipeline;
	}

	inline const FramePipeline& RenderSystem::GetFramePipeline() const
	{
		return *m_pipeline;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
