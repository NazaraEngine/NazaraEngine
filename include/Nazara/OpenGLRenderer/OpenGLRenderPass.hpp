// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLRENDERPASS_HPP
#define NAZARA_OPENGLRENDERER_OPENGLRENDERPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/RenderPass.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLRenderPass final : public RenderPass
	{
		public:
			using RenderPass::RenderPass;
			OpenGLRenderPass(const OpenGLRenderPass&) = delete;
			OpenGLRenderPass(OpenGLRenderPass&&) noexcept = default;
			~OpenGLRenderPass() = default;

			OpenGLRenderPass& operator=(const OpenGLRenderPass&) = delete;
			OpenGLRenderPass& operator=(OpenGLRenderPass&&) noexcept = default;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLRenderPass.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLRENDERPASS_HPP
