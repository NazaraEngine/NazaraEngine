// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_COMMANDPOOL_HPP
#define NAZARA_RENDERER_COMMANDPOOL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/CommandBuffer.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <functional>

namespace Nz
{
	class CommandBufferBuilder;

	class NAZARA_RENDERER_API CommandPool
	{
		public:
			CommandPool() = default;
			CommandPool(const CommandPool&) = delete;
			CommandPool(CommandPool&&) = default;
			virtual ~CommandPool();

			virtual CommandBufferPtr BuildCommandBuffer(const std::function<void(CommandBufferBuilder& builder)>& callback) = 0;

			virtual void UpdateDebugName(std::string_view name) = 0;

			CommandPool& operator=(const CommandPool&) = delete;
			CommandPool& operator=(CommandPool&&) = default;
	};
}

#include <Nazara/Renderer/CommandPool.inl>

#endif // NAZARA_RENDERER_COMMANDPOOL_HPP
