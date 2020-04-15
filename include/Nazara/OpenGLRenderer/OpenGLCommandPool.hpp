// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLCOMMANDPOOL_HPP
#define NAZARA_OPENGLRENDERER_OPENGLCOMMANDPOOL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/CommandPool.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/CommandPool.hpp>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLCommandPool final : public CommandPool
	{
		public:
			inline OpenGLCommandPool(Vk::Device& device, QueueType queueType);
			inline OpenGLCommandPool(Vk::Device& device, UInt32 queueFamilyIndex);
			OpenGLCommandPool(const OpenGLCommandPool&) = delete;
			OpenGLCommandPool(OpenGLCommandPool&&) noexcept = default;
			~OpenGLCommandPool() = default;

			std::unique_ptr<CommandBuffer> BuildCommandBuffer(const std::function<void(CommandBufferBuilder& builder)>& callback) override;

			OpenGLCommandPool& operator=(const OpenGLCommandPool&) = delete;
			OpenGLCommandPool& operator=(OpenGLCommandPool&&) = delete;

		private:
			Vk::CommandPool m_commandPool;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLCommandPool.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLCOMMANDPOOL_HPP
