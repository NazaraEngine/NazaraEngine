// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_FRAMEBUFFER_HPP
#define NAZARA_RENDERER_FRAMEBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <string_view>

namespace Nz
{
	class NAZARA_RENDERER_API Framebuffer
	{
		public:
			inline Framebuffer(FramebufferType type);
			Framebuffer(const Framebuffer&) = delete;
			Framebuffer(Framebuffer&&) noexcept = default;
			virtual ~Framebuffer();

			inline FramebufferType GetType() const;

			virtual void UpdateDebugName(std::string_view name) = 0;

			Framebuffer& operator=(const Framebuffer&) = delete;
			Framebuffer& operator=(Framebuffer&&) noexcept = default;

		private:
			FramebufferType m_type;
	};
}

#include <Nazara/Renderer/Framebuffer.inl>

#endif // NAZARA_RENDERER_FRAMEBUFFER_HPP
