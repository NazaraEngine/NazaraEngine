// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLRENDERIMAGE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLRENDERIMAGE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/RenderImage.hpp>
#include <Nazara/OpenGLRenderer/OpenGLUploadPool.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/CommandBuffer.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/CommandPool.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Fence.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Semaphore.hpp>
#include <vector>

namespace Nz
{
	class VkRenderWindow;

	class NAZARA_OPENGLRENDERER_API OpenGLRenderImage : public RenderImage
	{
		public:
			OpenGLRenderImage(VkRenderWindow& owner);
			OpenGLRenderImage(const OpenGLRenderImage&) = delete;
			OpenGLRenderImage(OpenGLRenderImage&&) noexcept = default;
			~OpenGLRenderImage();

			void Execute(const std::function<void(CommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags) override;

			inline Vk::Fence& GetInFlightFence();
			inline Vk::Semaphore& GetImageAvailableSemaphore();
			inline UInt32 GetImageIndex();
			inline Vk::Semaphore& GetRenderFinishedSemaphore();
			OpenGLUploadPool& GetUploadPool() override;

			void SubmitCommandBuffer(CommandBuffer* commandBuffer, QueueTypeFlags queueTypeFlags) override;
			void SubmitCommandBuffer(VkCommandBuffer commandBuffer, QueueTypeFlags queueTypeFlags);

			void Present() override;

			inline void Reset(UInt32 imageIndex);

			OpenGLRenderImage& operator=(const OpenGLRenderImage&) = delete;
			OpenGLRenderImage& operator=(OpenGLRenderImage&&) = delete;

		private:
			std::size_t m_currentCommandBuffer;
			std::vector<Vk::AutoCommandBuffer> m_inFlightCommandBuffers;
			std::vector<VkCommandBuffer> m_graphicalCommandsBuffers;
			VkRenderWindow& m_owner;
			Vk::CommandPool m_commandPool;
			Vk::Fence m_inFlightFence;
			Vk::Semaphore m_imageAvailableSemaphore;
			Vk::Semaphore m_renderFinishedSemaphore;
			OpenGLUploadPool m_uploadPool;
			UInt32 m_imageIndex;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLRenderImage.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLRENDERIMAGE_HPP
