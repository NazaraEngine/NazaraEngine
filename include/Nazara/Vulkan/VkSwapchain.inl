// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkSwapchain.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Vulkan/VkDevice.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline bool Swapchain::AcquireNextImage(Nz::UInt64 timeout, VkSemaphore semaphore, VkFence fence, UInt32* imageIndex)
		{
			m_lastErrorCode = m_device->vkAcquireNextImageKHR(*m_device, m_handle, timeout, semaphore, fence, imageIndex);
			switch (m_lastErrorCode)
			{
				case VkResult::VK_SUBOPTIMAL_KHR:
				case VkResult::VK_SUCCESS:
					return true;

				default:
					return false;
			}
		}

		inline bool Swapchain::Create(const DeviceHandle& device, const VkSwapchainCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator)
		{
			if (!DeviceObject::Create(device, createInfo, allocator))
				return false;

			UInt32 imageCount = 0;
			m_lastErrorCode = m_device->vkGetSwapchainImagesKHR(*m_device, m_handle, &imageCount, nullptr);
			if (m_lastErrorCode != VkResult::VK_SUCCESS || imageCount == 0)
			{
				NazaraError("Failed to query swapchain image count");
				return false;
			}

			m_images.resize(imageCount);
			m_lastErrorCode = m_device->vkGetSwapchainImagesKHR(*m_device, m_handle, &imageCount, m_images.data());
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to query swapchain images");
				return false;
			}

			return true;
		}

		inline VkImage Swapchain::GetImage(UInt32 index) const
		{
			return m_images[index];
		}

		inline const std::vector<VkImage>& Swapchain::GetImages() const
		{
			return m_images;
		}

		inline UInt32 Swapchain::GetImageCount() const
		{
			return m_images.size();
		}

		inline bool Swapchain::IsSupported() const
		{
			if (!m_device->IsExtensionLoaded("VK_KHR_swapchain"))
				return false;
		}

		VkResult Swapchain::CreateHelper(const DeviceHandle& device, const VkSwapchainCreateInfoKHR* createInfo, const VkAllocationCallbacks* allocator, VkSwapchainKHR* handle)
		{
			return device->vkCreateSwapchainKHR(*device, createInfo, allocator, handle);
		}

		void Swapchain::DestroyHelper(const DeviceHandle& device, VkSwapchainKHR handle, const VkAllocationCallbacks* allocator)
		{
			return device->vkDestroySwapchainKHR(*device, handle, allocator);
		}
	}
}

#include <Nazara/Vulkan/DebugOff.hpp>
