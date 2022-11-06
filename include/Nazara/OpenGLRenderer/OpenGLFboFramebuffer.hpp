// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLFBOFRAMEBUFFER_HPP
#define NAZARA_OPENGLRENDERER_OPENGLFBOFRAMEBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/OpenGLFramebuffer.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Framebuffer.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class OpenGLDevice;
	class RenderPass;
	class Texture;

	class NAZARA_OPENGLRENDERER_API OpenGLFboFramebuffer final : public OpenGLFramebuffer
	{
		public:
			OpenGLFboFramebuffer(OpenGLDevice& device, const std::vector<std::shared_ptr<Texture>>& attachments);
			OpenGLFboFramebuffer(const OpenGLFboFramebuffer&) = delete;
			OpenGLFboFramebuffer(OpenGLFboFramebuffer&&) noexcept = default;
			~OpenGLFboFramebuffer() = default;

			void Activate() const override;

			inline const Vector2ui& GetAttachmentSize(std::size_t i) const;

			std::size_t GetColorBufferCount() const override;

			const Vector2ui& GetSize() const override;

			OpenGLFboFramebuffer& operator=(const OpenGLFboFramebuffer&) = delete;
			OpenGLFboFramebuffer& operator=(OpenGLFboFramebuffer&&) = delete;

		private:
			GL::Framebuffer m_framebuffer;
			std::size_t m_colorAttachmentCount;
			std::vector<Vector2ui> m_attachmentSizes;
			Vector2ui m_size;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLFboFramebuffer.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLFBOFRAMEBUFFER_HPP
