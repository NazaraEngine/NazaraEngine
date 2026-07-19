// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLCOMMANDPOOL_HPP
#define NAZARA_OPENGLRENDERER_OPENGLCOMMANDPOOL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Export.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandBuffer.hpp>
#include <Nazara/Renderer/GpuCommandPool.hpp>
#include <NazaraUtils/Bitset.hpp>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLCommandPool final : public GpuCommandPool
	{
		friend OpenGLCommandBuffer;

		public:
			OpenGLCommandPool() = default;
			OpenGLCommandPool(const OpenGLCommandPool&) = delete;
			OpenGLCommandPool(OpenGLCommandPool&&) noexcept = default;
			~OpenGLCommandPool() = default;

			GpuCommandBufferPtr BuildPrimaryCommandBuffer(const FunctionRef<void(GpuCommandBufferBuilder& builder)>& callback) override;
			GpuCommandBufferPtr BuildSecondaryCommandBuffer(const FunctionRef<void(GpuCommandBufferBuilder& builder)>& callback) override;

			void UpdateDebugName(std::string_view name) override;

			OpenGLCommandPool& operator=(const OpenGLCommandPool&) = delete;
			OpenGLCommandPool& operator=(OpenGLCommandPool&&) = delete;

		private:
			struct CommandPool;

			CommandPool& AllocatePool();
			GpuCommandBufferPtr AllocateFromPool(std::size_t poolIndex);
			GpuCommandBufferPtr BuildCommandBuffer(const FunctionRef<void(GpuCommandBufferBuilder& builder)>& callback);
			void Release(GpuCommandBuffer& commandBuffer);
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
