// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERBUFFER_HPP
#define NAZARA_RENDERER_RENDERBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Buffer.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <NazaraUtils/Signal.hpp>
#include <memory>
#include <string_view>

namespace Nz
{
	class GpuAsyncCommands;
	class GpuDevice;

	class NAZARA_RENDERER_API GpuBuffer : public Buffer
	{
		public:
			inline GpuBuffer(GpuDevice& renderDevice, UInt64 size, BufferUsageFlags usage);
			GpuBuffer(const GpuBuffer&) = delete;
			GpuBuffer(GpuBuffer&&) = delete;
			~GpuBuffer();

			using Buffer::Fill;
			virtual bool Fill(GpuAsyncCommands& asyncTransfer, const void* data, UInt64 offset, UInt64 size) = 0;

			inline GpuDevice& GetRenderDevice();
			inline const GpuDevice& GetRenderDevice() const;

			virtual void UpdateDebugName(std::string_view name) = 0;

			GpuBuffer& operator=(const GpuBuffer&) = delete;
			GpuBuffer& operator=(GpuBuffer&&) = delete;

			NazaraSignal(OnGpuBufferRelease, GpuBuffer* /*renderBuffer*/);

		private:
			GpuDevice& m_renderDevice;
	};

	NAZARA_RENDERER_API BufferFactory GetGpuBufferFactory(std::shared_ptr<GpuDevice> device);
}

#include <Nazara/Renderer/GpuBuffer.inl>

#endif // NAZARA_RENDERER_RENDERBUFFER_HPP
