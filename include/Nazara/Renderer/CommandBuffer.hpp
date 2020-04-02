// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_COMMANDBUFFER_HPP
#define NAZARA_COMMANDBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API CommandBuffer
	{
		public:
			CommandBuffer() = default;
			CommandBuffer(const CommandBuffer&) = delete;
			CommandBuffer(CommandBuffer&&) = default;
			virtual ~CommandBuffer();

			CommandBuffer& operator=(const CommandBuffer&) = delete;
			CommandBuffer& operator=(CommandBuffer&&) = default;
	};
}

#include <Nazara/Renderer/CommandBuffer.inl>

#endif // NAZARA_COMMANDBUFFER_HPP
