// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
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
