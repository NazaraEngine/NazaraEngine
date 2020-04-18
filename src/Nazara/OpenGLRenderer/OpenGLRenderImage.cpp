// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLRenderImage.hpp>
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

	void OpenGLRenderImage::Execute(const std::function<void(CommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags)
	{
	}

	OpenGLUploadPool& OpenGLRenderImage::GetUploadPool()
	{
		return m_uploadPool;
	}

	void OpenGLRenderImage::SubmitCommandBuffer(CommandBuffer* commandBuffer, QueueTypeFlags queueTypeFlags)
	{
	}

	void OpenGLRenderImage::Present()
	{
		m_owner.Present();
	}
}
