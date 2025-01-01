// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_COMMANDBUFFER_HPP
#define NAZARA_RENDERER_COMMANDBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <memory>
#include <string_view>

namespace Nz
{
	class CommandBuffer;
	class CommandBufferDeleter;

	using CommandBufferPtr = std::unique_ptr<CommandBuffer, CommandBufferDeleter>;

	class NAZARA_RENDERER_API CommandBuffer
	{
		friend CommandBufferDeleter;

		public:
			CommandBuffer() = default;
			CommandBuffer(const CommandBuffer&) = delete;
			CommandBuffer(CommandBuffer&&) = delete;
			virtual ~CommandBuffer();

			virtual void UpdateDebugName(std::string_view name) = 0;

			CommandBuffer& operator=(const CommandBuffer&) = delete;
			CommandBuffer& operator=(CommandBuffer&&) = delete;

		protected:
			virtual void Release() = 0;
	};

	class CommandBufferDeleter
	{
		public:
			inline void operator()(CommandBuffer* commandBuffer);
	};
}

#include <Nazara/Renderer/CommandBuffer.inl>

#endif // NAZARA_RENDERER_COMMANDBUFFER_HPP
