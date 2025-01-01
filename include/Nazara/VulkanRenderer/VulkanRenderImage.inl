// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp


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

	inline Vk::Semaphore& VulkanRenderImage::GetRenderFinishedSemaphore()
	{
		return m_renderFinishedSemaphore;
	}

	inline void VulkanRenderImage::Reset(UInt32 imageIndex)
	{
		FlushReleaseQueue();

		m_graphicalCommandBuffers.clear();
		m_freeCommandBufferIndex = 0;
		m_imageIndex = imageIndex;
		m_commandPool.Reset();
		m_uploadPool.Reset();
	}
}
