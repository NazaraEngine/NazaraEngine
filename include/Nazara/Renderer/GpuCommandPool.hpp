// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_GPUCOMMANDPOOL_HPP
#define NAZARA_RENDERER_GPUCOMMANDPOOL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <Nazara/Renderer/GpuCommandBuffer.hpp>
#include <NazaraUtils/FunctionRef.hpp>

namespace Nz
{
	class GpuCommandBufferBuilder;

	class NAZARA_RENDERER_API GpuCommandPool
	{
		public:
			GpuCommandPool() = default;
			GpuCommandPool(const GpuCommandPool&) = delete;
			GpuCommandPool(GpuCommandPool&&) = default;
			virtual ~GpuCommandPool();

			virtual GpuCommandBufferPtr BuildPrimaryCommandBuffer(const FunctionRef<void(GpuCommandBufferBuilder& builder)>& callback) = 0;
			virtual GpuCommandBufferPtr BuildSecondaryCommandBuffer(const FunctionRef<void(GpuCommandBufferBuilder& builder)>& callback) = 0;

			virtual void UpdateDebugName(std::string_view name) = 0;

			GpuCommandPool& operator=(const GpuCommandPool&) = delete;
			GpuCommandPool& operator=(GpuCommandPool&&) = default;
	};
}

#include <Nazara/Renderer/GpuCommandPool.inl>

#endif // NAZARA_RENDERER_GPUCOMMANDPOOL_HPP
