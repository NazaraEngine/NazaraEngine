// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLCommandPool.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandBufferBuilder.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	std::unique_ptr<CommandBuffer> OpenGLCommandPool::BuildCommandBuffer(const std::function<void(CommandBufferBuilder& builder)>& callback)
	{
		std::unique_ptr<OpenGLCommandBuffer> commandBuffer = std::make_unique<OpenGLCommandBuffer>();

		OpenGLCommandBufferBuilder builder(*commandBuffer);
		callback(builder);

		return commandBuffer;
	}
}
