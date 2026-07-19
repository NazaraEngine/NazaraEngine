// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_GPUFRAMEBUFFER_HPP
#define NAZARA_RENDERER_GPUFRAMEBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <string_view>

namespace Nz
{
	class NAZARA_RENDERER_API GpuFramebuffer
	{
		public:
			inline GpuFramebuffer(FramebufferType type);
			GpuFramebuffer(const GpuFramebuffer&) = delete;
			GpuFramebuffer(GpuFramebuffer&&) noexcept = default;
			virtual ~GpuFramebuffer();

			inline FramebufferType GetType() const;

			virtual void UpdateDebugName(std::string_view name) = 0;

			GpuFramebuffer& operator=(const GpuFramebuffer&) = delete;
			GpuFramebuffer& operator=(GpuFramebuffer&&) noexcept = default;

		private:
			FramebufferType m_type;
	};
}

#include <Nazara/Renderer/GpuFramebuffer.inl>

#endif // NAZARA_RENDERER_GPUFRAMEBUFFER_HPP
