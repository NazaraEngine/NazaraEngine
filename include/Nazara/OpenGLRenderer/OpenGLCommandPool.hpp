// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLCOMMANDPOOL_HPP
#define NAZARA_OPENGLRENDERER_OPENGLCOMMANDPOOL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Renderer/CommandPool.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandBuffer.hpp>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLCommandPool final : public CommandPool
	{
		friend OpenGLCommandBuffer;

		public:
			OpenGLCommandPool() = default;
			OpenGLCommandPool(const OpenGLCommandPool&) = delete;
			OpenGLCommandPool(OpenGLCommandPool&&) noexcept = default;
			~OpenGLCommandPool() = default;

			CommandBufferPtr BuildCommandBuffer(const std::function<void(CommandBufferBuilder& builder)>& callback) override;

			OpenGLCommandPool& operator=(const OpenGLCommandPool&) = delete;
			OpenGLCommandPool& operator=(OpenGLCommandPool&&) = delete;

		private:
			struct CommandPool;

			CommandPool& AllocatePool();
			CommandBufferPtr AllocateFromPool(std::size_t poolIndex);
			void Release(CommandBuffer& commandBuffer);
			inline void TryToShrink();

			struct CommandPool
			{
				using BindingStorage = std::aligned_storage_t<sizeof(OpenGLCommandBuffer), alignof(OpenGLCommandBuffer)>;

				Bitset<UInt64> freeCommands;
				std::unique_ptr<BindingStorage[]> storage;
			};

			std::vector<CommandPool> m_commandPools;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLCommandPool.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLCOMMANDPOOL_HPP
