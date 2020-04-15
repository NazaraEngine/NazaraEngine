// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLRENDERPASS_HPP
#define NAZARA_OPENGLRENDERER_OPENGLRENDERPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/RenderPass.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/RenderPass.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLRenderPass final : public RenderPass
	{
		public:
			inline OpenGLRenderPass(Vk::RenderPass renderPass, std::initializer_list<PixelFormat> formats); //< FIXME
			OpenGLRenderPass(const OpenGLRenderPass&) = delete;
			OpenGLRenderPass(OpenGLRenderPass&&) noexcept = default;
			~OpenGLRenderPass() = default;

			inline PixelFormat GetAttachmentFormat(std::size_t attachmentIndex) const;
			inline Vk::RenderPass& GetRenderPass();
			inline const Vk::RenderPass& GetRenderPass() const;

			OpenGLRenderPass& operator=(const OpenGLRenderPass&) = delete;
			OpenGLRenderPass& operator=(OpenGLRenderPass&&) noexcept = default;

		private:
			std::vector<PixelFormat> m_formats;
			Vk::RenderPass m_renderPass;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLRenderPass.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLRENDERPASS_HPP
