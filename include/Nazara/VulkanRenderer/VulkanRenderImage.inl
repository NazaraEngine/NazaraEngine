// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanRenderImage.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline Vk::Fence& VulkanRenderImage::GetInFlightFence()
	{
		return m_inFlightFence;
	}

	inline Vk::Semaphore& VulkanRenderImage::GetImageAvailableSemaphore()
	{
		return m_imageAvailableSemaphore;
	}

	inline UInt32 VulkanRenderImage::GetImageIndex()
	{
		return m_imageIndex;
	}

	inline Vk::Semaphore& VulkanRenderImage::GetRenderFinishedSemaphore()
	{
		return m_renderFinishedSemaphore;
	}

	inline void VulkanRenderImage::Reset(UInt32 imageIndex)
	{
		FlushReleaseQueue();

		m_graphicalCommandsBuffers.clear();
		m_currentCommandBuffer = 0;
		m_imageIndex = imageIndex;
		m_uploadPool.Reset();
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
