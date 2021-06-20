// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FRAMEBUFFER_HPP
#define NAZARA_FRAMEBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API Framebuffer
	{
		public:
			Framebuffer() = default;
			Framebuffer(const Framebuffer&) = delete;
			Framebuffer(Framebuffer&&) noexcept = default;
			virtual ~Framebuffer();

			Framebuffer& operator=(const Framebuffer&) = delete;
			Framebuffer& operator=(Framebuffer&&) noexcept = default;
	};
}

#include <Nazara/Renderer/Framebuffer.inl>

#endif // NAZARA_FRAMEBUFFER_HPP
