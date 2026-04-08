// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_ASYNCRENDERCOMMANDS_HPP
#define NAZARA_RENDERER_ASYNCRENDERCOMMANDS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <functional>
#include <vector>

namespace Nz
{
	class CommandBufferBuilder;

	class NAZARA_RENDERER_API AsyncRenderCommands
	{
		public:
			using CompletionCallback = std::function<void()>;

			AsyncRenderCommands() = default;
			AsyncRenderCommands(const AsyncRenderCommands&) = delete;
			AsyncRenderCommands(AsyncRenderCommands&&) = delete;
			virtual ~AsyncRenderCommands();

			virtual void AddCommands(Nz::FunctionRef<void(CommandBufferBuilder& builder)> callback) = 0;
			inline void AddCompletionCallback(CompletionCallback&& callback);

			AsyncRenderCommands& operator=(const AsyncRenderCommands&) = delete;
			AsyncRenderCommands& operator=(AsyncRenderCommands&&) = delete;

		protected:
			void TriggerCallbacks();

		private:
			std::vector<CompletionCallback> m_completionCallbacks;
	};
}

#include <Nazara/Renderer/AsyncRenderCommands.inl>

#endif // NAZARA_RENDERER_ASYNCRENDERCOMMANDS_HPP
