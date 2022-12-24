// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLComputePipeline.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline const ComputePipelineInfo& OpenGLComputePipeline::GetPipelineInfo() const
	{
		return m_pipelineInfo;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
