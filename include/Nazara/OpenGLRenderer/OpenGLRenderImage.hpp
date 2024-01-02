// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLRENDERIMAGE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLRENDERIMAGE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/OpenGLUploadPool.hpp>
#include <Nazara/Renderer/RenderImage.hpp>
#include <vector>

namespace Nz
{
	class OpenGLCommandBuffer;
	class OpenGLSwapchain;

	class NAZARA_OPENGLRENDERER_API OpenGLRenderImage : public RenderImage
	{
		public:
			OpenGLRenderImage(OpenGLSwapchain& owner);

			void Execute(const FunctionRef<void(CommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags) override;

			UInt32 GetImageIndex() const override;
			OpenGLUploadPool& GetUploadPool() override;

			void Present() override;

			inline void Reset(UInt32 imageIndex);

			void SubmitCommandBuffer(CommandBuffer* commandBuffer, QueueTypeFlags queueTypeFlags) override;

		private:
			OpenGLSwapchain& m_owner;
			OpenGLUploadPool m_uploadPool;
			UInt32 m_imageIndex;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLRenderImage.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLRENDERIMAGE_HPP
