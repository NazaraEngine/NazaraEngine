// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLRENDERPASS_HPP
#define NAZARA_OPENGLRENDERER_OPENGLRENDERPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Export.hpp>
#include <Nazara/Renderer/RenderPass.hpp>
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

			void UpdateDebugName(std::string_view name) override;

			OpenGLRenderPass& operator=(const OpenGLRenderPass&) = delete;
			OpenGLRenderPass& operator=(OpenGLRenderPass&&) noexcept = default;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLRenderPass.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLRENDERPASS_HPP
