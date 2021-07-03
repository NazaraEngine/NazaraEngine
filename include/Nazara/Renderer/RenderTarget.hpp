// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERTARGET_HPP
#define NAZARA_RENDERTARGET_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>

namespace Nz
{
	class Framebuffer;
	class RenderPass;

	class NAZARA_RENDERER_API RenderTarget
	{
		public:
			RenderTarget() = default;
			virtual ~RenderTarget();

			virtual const Framebuffer& GetFramebuffer(std::size_t i) const = 0;
			virtual std::size_t GetFramebufferCount() const = 0;
			virtual const RenderPass& GetRenderPass() const = 0;
	};
}

#include <Nazara/Renderer/RenderTarget.inl>

#endif
