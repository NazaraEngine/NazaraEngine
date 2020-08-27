// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_COMMANDBUFFER_HPP
#define NAZARA_COMMANDBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <memory>

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

#endif // NAZARA_COMMANDBUFFER_HPP
