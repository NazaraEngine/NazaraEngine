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
	class GpuCommandBufferBuilder;

	class NAZARA_RENDERER_API GpuAsyncCommands
	{
		public:
			using CompletionCallback = std::function<void()>;

			GpuAsyncCommands() = default;
			GpuAsyncCommands(const GpuAsyncCommands&) = delete;
			GpuAsyncCommands(GpuAsyncCommands&&) = delete;
			virtual ~GpuAsyncCommands();

			virtual void AddCommands(Nz::FunctionRef<void(GpuCommandBufferBuilder& builder)> callback) = 0;
			inline void AddCompletionCallback(CompletionCallback&& callback);

			GpuAsyncCommands& operator=(const GpuAsyncCommands&) = delete;
			GpuAsyncCommands& operator=(GpuAsyncCommands&&) = delete;

		protected:
			void TriggerCallbacks();

		private:
			std::vector<CompletionCallback> m_completionCallbacks;
	};
}

#include <Nazara/Renderer/GpuAsyncCommands.inl>

#endif // NAZARA_RENDERER_ASYNCRENDERCOMMANDS_HPP
