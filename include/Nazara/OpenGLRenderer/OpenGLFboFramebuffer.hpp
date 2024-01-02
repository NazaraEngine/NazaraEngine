// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLFBOFRAMEBUFFER_HPP
#define NAZARA_OPENGLRENDERER_OPENGLFBOFRAMEBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/OpenGLFramebuffer.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Framebuffer.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Nz
{
	class OpenGLDevice;
	class RenderPass;
	class Texture;

	class NAZARA_OPENGLRENDERER_API OpenGLFboFramebuffer final : public OpenGLFramebuffer
	{
		public:
			OpenGLFboFramebuffer(OpenGLDevice& device, std::vector<std::shared_ptr<Texture>> attachments);
			OpenGLFboFramebuffer(const OpenGLFboFramebuffer&) = delete;
			OpenGLFboFramebuffer(OpenGLFboFramebuffer&&) = delete;
			~OpenGLFboFramebuffer() = default;

			void Activate() const override;

			inline Vector2ui GetAttachmentSize(std::size_t i) const;

			std::size_t GetColorBufferCount() const override;

			const Vector2ui& GetSize() const override;

			void UpdateDebugName(std::string_view name) override;

			OpenGLFboFramebuffer& operator=(const OpenGLFboFramebuffer&) = delete;
			OpenGLFboFramebuffer& operator=(OpenGLFboFramebuffer&&) = delete;

		private:
			GL::Framebuffer& CreateFramebuffer(const GL::Context& context) const;

			struct ContextFramebuffer
			{
				GL::Framebuffer framebuffer;

				NazaraSlot(GL::Context, OnContextDestruction, onContextDestruction);
			};

			std::size_t m_colorAttachmentCount;
			std::string m_debugName;
			std::vector<std::shared_ptr<Texture>> m_attachments;
			mutable std::unordered_map<const GL::Context*, ContextFramebuffer> m_framebuffers;
			OpenGLDevice& m_device;
			Vector2ui m_size;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLFboFramebuffer.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLFBOFRAMEBUFFER_HPP
