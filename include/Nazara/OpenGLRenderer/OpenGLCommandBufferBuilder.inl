// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLCommandBufferBuilder.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline OpenGLCommandBufferBuilder::OpenGLCommandBufferBuilder(Vk::CommandBuffer& commandBuffer, std::size_t imageIndex) :
	m_commandBuffer(commandBuffer),
	m_framebufferCount(0),
	m_imageIndex(imageIndex)
	{
	}

	inline Vk::CommandBuffer& OpenGLCommandBufferBuilder::GetCommandBuffer()
	{
		return m_commandBuffer;
	}

	inline std::size_t OpenGLCommandBufferBuilder::GetMaxFramebufferCount() const
	{
		return m_framebufferCount;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
