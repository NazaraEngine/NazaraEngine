// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLRenderImage.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline Vk::Fence& Nz::OpenGLRenderImage::GetInFlightFence()
	{
		return m_inFlightFence;
	}

	inline Vk::Semaphore& OpenGLRenderImage::GetImageAvailableSemaphore()
	{
		return m_imageAvailableSemaphore;
	}

	inline UInt32 OpenGLRenderImage::GetImageIndex()
	{
		return m_imageIndex;
	}

	inline Vk::Semaphore& OpenGLRenderImage::GetRenderFinishedSemaphore()
	{
		return m_renderFinishedSemaphore;
	}

	inline void OpenGLRenderImage::Reset(UInt32 imageIndex)
	{
		m_graphicalCommandsBuffers.clear();
		m_currentCommandBuffer = 0;
		m_imageIndex = imageIndex;
		m_uploadPool.Reset();
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
