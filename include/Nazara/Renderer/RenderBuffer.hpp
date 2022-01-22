// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERBUFFER_HPP
#define NAZARA_RENDERER_RENDERBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <memory>

namespace Nz
{
	class RenderDevice;

	class NAZARA_RENDERER_API RenderBuffer : public Buffer
	{
		public:
			inline RenderBuffer(RenderDevice& renderDevice, BufferType type, UInt64 size, BufferUsageFlags usage);
			RenderBuffer(const RenderBuffer&) = delete;
			RenderBuffer(RenderBuffer&&) = delete;
			~RenderBuffer();

			inline RenderDevice& GetRenderDevice();
			inline const RenderDevice& GetRenderDevice() const;

			RenderBuffer& operator=(const RenderBuffer&) = delete;
			RenderBuffer& operator=(RenderBuffer&&) = delete;

		private:
			RenderDevice& m_renderDevice;
	};

	NAZARA_RENDERER_API BufferFactory GetRenderBufferFactory(std::shared_ptr<RenderDevice> device);
}

#include <Nazara/Renderer/RenderBuffer.inl>

#endif // NAZARA_RENDERER_RENDERBUFFER_HPP
