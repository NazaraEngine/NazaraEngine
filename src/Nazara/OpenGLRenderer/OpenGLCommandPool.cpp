// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if 0

#include <Nazara/OpenGLRenderer/OpenGLCommandPool.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandBufferBuilder.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/CommandBuffer.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	std::unique_ptr<CommandBuffer> OpenGLCommandPool::BuildCommandBuffer(const std::function<void(CommandBufferBuilder& builder)>& callback)
	{
		std::vector<Vk::AutoCommandBuffer> commandBuffers;
		auto BuildCommandBuffer = [&](std::size_t imageIndex)
		{
			Vk::AutoCommandBuffer& commandBuffer = commandBuffers.emplace_back(m_commandPool.AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY));

			if (!commandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT))
				throw std::runtime_error("failed to begin command buffer: " + TranslateOpenGLError(commandBuffer->GetLastErrorCode()));

			OpenGLCommandBufferBuilder builder(commandBuffer.Get(), imageIndex);
			callback(builder);

			if (!commandBuffer->End())
				throw std::runtime_error("failed to build command buffer: " + TranslateOpenGLError(commandBuffer->GetLastErrorCode()));

			return builder.GetMaxFramebufferCount();
		};

		std::size_t maxFramebufferCount = BuildCommandBuffer(0);
		for (std::size_t i = 1; i < maxFramebufferCount; ++i)
			BuildCommandBuffer(i);

		return std::make_unique<OpenGLCommandBuffer>(std::move(commandBuffers));
	}
}

#endif
