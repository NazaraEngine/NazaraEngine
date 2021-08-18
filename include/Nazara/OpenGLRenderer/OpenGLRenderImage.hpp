// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLRENDERIMAGE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLRENDERIMAGE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/RenderImage.hpp>
#include <Nazara/OpenGLRenderer/OpenGLUploadPool.hpp>
#include <vector>

namespace Nz
{
	class OpenGLCommandBuffer;
	class OpenGLRenderWindow;

	class NAZARA_OPENGLRENDERER_API OpenGLRenderImage : public RenderImage
	{
		public:
			OpenGLRenderImage(OpenGLRenderWindow& owner);
			OpenGLRenderImage(const OpenGLRenderImage&) = delete;
			OpenGLRenderImage(OpenGLRenderImage&&) noexcept = default;
			~OpenGLRenderImage() = default;

			void Execute(const std::function<void(CommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags) override;

			OpenGLUploadPool& GetUploadPool() override;

			void Present() override;

			void SubmitCommandBuffer(CommandBuffer* commandBuffer, QueueTypeFlags queueTypeFlags) override;

			OpenGLRenderImage& operator=(const OpenGLRenderImage&) = delete;
			OpenGLRenderImage& operator=(OpenGLRenderImage&&) = delete;

		private:
			OpenGLRenderWindow& m_owner;
			OpenGLUploadPool m_uploadPool;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLRenderImage.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLRENDERIMAGE_HPP
