// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLCommandBuffer.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline OpenGLCommandBuffer::OpenGLCommandBuffer(Vk::AutoCommandBuffer commandBuffer)
	{
		m_commandBuffers.push_back(std::move(commandBuffer));
	}

	inline OpenGLCommandBuffer::OpenGLCommandBuffer(std::vector<Vk::AutoCommandBuffer> commandBuffers) :
	m_commandBuffers(std::move(commandBuffers))
	{
	}

	inline Vk::CommandBuffer& OpenGLCommandBuffer::GetCommandBuffer(std::size_t imageIndex)
	{
		return m_commandBuffers[imageIndex].Get();
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
