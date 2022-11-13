// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLRENDERIMAGE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLRENDERIMAGE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/OpenGLUploadPool.hpp>
#include <Nazara/Renderer/RenderImage.hpp>
#include <vector>

namespace Nz
{
	class OpenGLCommandBuffer;
	class OpenGLRenderWindow;

	class NAZARA_OPENGLRENDERER_API OpenGLRenderImage : public RenderImage
	{
		public:
			OpenGLRenderImage(OpenGLRenderWindow& owner);

			void Execute(const std::function<void(CommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags) override;

			OpenGLUploadPool& GetUploadPool() override;

			void Present() override;

			void SubmitCommandBuffer(CommandBuffer* commandBuffer, QueueTypeFlags queueTypeFlags) override;

		private:
			OpenGLRenderWindow& m_owner;
			OpenGLUploadPool m_uploadPool;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLRenderImage.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLRENDERIMAGE_HPP
