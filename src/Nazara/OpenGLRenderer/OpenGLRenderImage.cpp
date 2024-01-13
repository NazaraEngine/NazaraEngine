// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLRenderImage.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandBufferBuilder.hpp>
#include <Nazara/OpenGLRenderer/OpenGLSwapchain.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLRenderImage::OpenGLRenderImage(OpenGLSwapchain& owner) :
	RenderImage(owner.GetDevice()),
	m_owner(owner),
	m_uploadPool(2 * 1024 * 1024)
	{
	}

	void OpenGLRenderImage::Execute(const FunctionRef<void(CommandBufferBuilder& builder)>& callback, QueueTypeFlags /*queueTypeFlags*/)
	{
		OpenGLCommandBuffer commandBuffer; //< TODO: Use a pool and remove default constructor
		OpenGLCommandBufferBuilder builder(commandBuffer);
		callback(builder);

		commandBuffer.Execute();
	}

	UInt32 OpenGLRenderImage::GetImageIndex() const
	{
		return m_imageIndex;
	}

	OpenGLUploadPool& OpenGLRenderImage::GetUploadPool()
	{
		return m_uploadPool;
	}

	void OpenGLRenderImage::Present()
	{
		m_owner.Present();
		m_uploadPool.Reset();
		FlushReleaseQueue();
	}

	void OpenGLRenderImage::SubmitCommandBuffer(CommandBuffer* commandBuffer, QueueTypeFlags /*queueTypeFlags*/)
	{
		OpenGLCommandBuffer* oglCommandBuffer = SafeCast<OpenGLCommandBuffer*>(commandBuffer);
		oglCommandBuffer->Execute();
	}
}
