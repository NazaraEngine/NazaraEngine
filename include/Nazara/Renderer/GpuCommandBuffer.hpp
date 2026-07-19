// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_GPUCOMMANDBUFFER_HPP
#define NAZARA_RENDERER_GPUCOMMANDBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <memory>
#include <string_view>

namespace Nz
{
	class GpuCommandBuffer;
	class GpuCommandBufferDeleter;

	using GpuCommandBufferPtr = std::unique_ptr<GpuCommandBuffer, GpuCommandBufferDeleter>;

	class NAZARA_RENDERER_API GpuCommandBuffer
	{
		friend GpuCommandBufferDeleter;

		public:
			GpuCommandBuffer() = default;
			GpuCommandBuffer(const GpuCommandBuffer&) = delete;
			GpuCommandBuffer(GpuCommandBuffer&&) = delete;
			virtual ~GpuCommandBuffer();

			virtual void UpdateDebugName(std::string_view name) = 0;

			GpuCommandBuffer& operator=(const GpuCommandBuffer&) = delete;
			GpuCommandBuffer& operator=(GpuCommandBuffer&&) = delete;

		protected:
			virtual void Release() = 0;
	};

	class GpuCommandBufferDeleter
	{
		public:
			inline void operator()(GpuCommandBuffer* commandBuffer);
	};
}

#include <Nazara/Renderer/GpuCommandBuffer.inl>

#endif // NAZARA_RENDERER_GPUCOMMANDBUFFER_HPP
