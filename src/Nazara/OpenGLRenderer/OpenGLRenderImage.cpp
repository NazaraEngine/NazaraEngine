// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLRenderImage.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandBufferBuilder.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderWindow.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLRenderImage::OpenGLRenderImage(OpenGLRenderWindow& owner) :
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
		OpenGLCommandBuffer* oglCommandBuffer = static_cast<OpenGLCommandBuffer*>(commandBuffer);
		oglCommandBuffer->Execute();
	}
}
