// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_FRAMEBUFFER_HPP
#define NAZARA_RENDERER_FRAMEBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>

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

			Framebuffer& operator=(const Framebuffer&) = delete;
			Framebuffer& operator=(Framebuffer&&) noexcept = default;

		private:
			FramebufferType m_type;
	};
}

#include <Nazara/Renderer/Framebuffer.inl>

#endif // NAZARA_RENDERER_FRAMEBUFFER_HPP
