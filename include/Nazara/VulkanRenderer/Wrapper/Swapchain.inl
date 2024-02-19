// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>

namespace Nz::Vk
{
	inline bool Swapchain::AcquireNextImage(Nz::UInt64 timeout, VkSemaphore semaphore, VkFence fence, UInt32* imageIndex) const
	{
		m_lastErrorCode = m_device->vkAcquireNextImageKHR(*m_device, m_handle, timeout, semaphore, fence, imageIndex);
		switch (m_lastErrorCode)
		{
			case VkResult::VK_SUBOPTIMAL_KHR:
			case VkResult::VK_SUCCESS:
				return true;

			default:
			{
				NazaraErrorFmt("failed to acquire next swapchain image: {0}", TranslateVulkanError(m_lastErrorCode));
				return false;
			}
		}
	}

	inline bool Swapchain::Create(Device& device, const VkSwapchainCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator)
	{
		if (!DeviceObject::Create(device, createInfo, allocator))
			return false;

		UInt32 imageCount = 0;
		m_lastErrorCode = m_device->vkGetSwapchainImagesKHR(*m_device, m_handle, &imageCount, nullptr);
		if (m_lastErrorCode != VkResult::VK_SUCCESS || imageCount == 0)
		{
			NazaraErrorFmt("failed to query swapchain image count: {0}", TranslateVulkanError(m_lastErrorCode));
			return false;
		}

		std::vector<VkImage> images(imageCount);
		m_lastErrorCode = m_device->vkGetSwapchainImagesKHR(*m_device, m_handle, &imageCount, images.data());
		if (m_lastErrorCode != VkResult::VK_SUCCESS)
		{
			NazaraErrorFmt("failed to query swapchain images: {0}", TranslateVulkanError(m_lastErrorCode));
			return false;
		}

		m_images.resize(imageCount);
		for (UInt32 i = 0; i < imageCount; ++i)
		{
			m_images[i].image = images[i];

			VkImageViewCreateInfo imageViewCreateInfo = {
				VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, // VkStructureType            sType;
				nullptr,                                  // const void*                pNext;
				0,                                        // VkImageViewCreateFlags     flags;
				m_images[i].image,                       // VkImage                    image;
				VK_IMAGE_VIEW_TYPE_2D,                    // VkImageViewType            viewType;
				createInfo.imageFormat,                   // VkFormat                   format;
				{                                         // VkComponentMapping         components;
					VK_COMPONENT_SWIZZLE_R,               // VkComponentSwizzle         .r;
					VK_COMPONENT_SWIZZLE_G,               // VkComponentSwizzle         .g;
					VK_COMPONENT_SWIZZLE_B,               // VkComponentSwizzle         .b;
					VK_COMPONENT_SWIZZLE_A                // VkComponentSwizzle         .a;
				},
				{                                         // VkImageSubresourceRange    subresourceRange;
					VK_IMAGE_ASPECT_COLOR_BIT,            // VkImageAspectFlags         .aspectMask;
					0,                                    // uint32_t                   .baseMipLevel;
					1,                                    // uint32_t                   .levelCount;
					0,                                    // uint32_t                   .baseArrayLayer;
					1                                     // uint32_t                   .layerCount;
				}
			};

			if (!m_images[i].view.Create(*m_device, imageViewCreateInfo))
			{
				NazaraErrorFmt("failed to create image view for image #{0}", i);
				return false;
			}
		}

		return true;
	}

	inline const Swapchain::Image& Swapchain::GetImage(UInt32 index) const
	{
		return m_images[index];
	}

	inline const std::vector<Swapchain::Image>& Swapchain::GetImages() const
	{
		return m_images;
	}

	inline UInt32 Swapchain::GetImageCount() const
	{
		return static_cast<UInt32>(m_images.size());
	}

	inline bool Swapchain::IsSupported() const
	{
		if (!m_device->IsExtensionLoaded(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
			return false;

		return true;
	}

	inline VkResult Swapchain::CreateHelper(Device& device, const VkSwapchainCreateInfoKHR* createInfo, const VkAllocationCallbacks* allocator, VkSwapchainKHR* handle)
	{
		return device.vkCreateSwapchainKHR(device, createInfo, allocator, handle);
	}

	inline void Swapchain::DestroyHelper(Device& device, VkSwapchainKHR handle, const VkAllocationCallbacks* allocator)
	{
		return device.vkDestroySwapchainKHR(device, handle, allocator);
	}
}

