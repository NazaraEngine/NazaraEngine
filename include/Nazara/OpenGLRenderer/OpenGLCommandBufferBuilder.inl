// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLCommandBufferBuilder.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline OpenGLCommandBufferBuilder::OpenGLCommandBufferBuilder(OpenGLCommandBuffer& commandBuffer) :
	m_commandBuffer(commandBuffer)
	{
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
