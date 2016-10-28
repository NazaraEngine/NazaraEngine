// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/Swapchain.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
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
					NazaraError("Failed to acquire next swapchain image: " + TranslateVulkanError(m_lastErrorCode));
					return false;
				}
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
				NazaraError("Failed to query swapchain image count: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			std::vector<VkImage> images(imageCount);
			m_lastErrorCode = m_device->vkGetSwapchainImagesKHR(*m_device, m_handle, &imageCount, images.data());
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to query swapchain images: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			m_buffers.resize(imageCount);
			for (UInt32 i = 0; i < imageCount; ++i)
			{
				m_buffers[i].image = images[i];

				VkImageViewCreateInfo imageViewCreateInfo = {
					VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, // VkStructureType            sType;
					nullptr,                                  // const void*                pNext;
					0,                                        // VkImageViewCreateFlags     flags;
					m_buffers[i].image,                       // VkImage                    image;
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

				if (!m_buffers[i].view.Create(m_device, imageViewCreateInfo))
				{
					NazaraError("Failed to create image view for image #" + String::Number(i));
					return false;
				}
			}

			return true;
		}

		inline const Swapchain::Buffer& Swapchain::GetBuffer(UInt32 index) const
		{
			return m_buffers[index];
		}

		inline const std::vector<Swapchain::Buffer>& Swapchain::GetBuffers() const
		{
			return m_buffers;
		}

		inline UInt32 Swapchain::GetBufferCount() const
		{
			return static_cast<UInt32>(m_buffers.size());
		}

		inline bool Swapchain::IsSupported() const
		{
			if (!m_device->IsExtensionLoaded("VK_KHR_swapchain"))
				return false;

			return true;
		}

		inline VkResult Swapchain::CreateHelper(const DeviceHandle& device, const VkSwapchainCreateInfoKHR* createInfo, const VkAllocationCallbacks* allocator, VkSwapchainKHR* handle)
		{
			return device->vkCreateSwapchainKHR(*device, createInfo, allocator, handle);
		}

		inline void Swapchain::DestroyHelper(const DeviceHandle& device, VkSwapchainKHR handle, const VkAllocationCallbacks* allocator)
		{
			return device->vkDestroySwapchainKHR(*device, handle, allocator);
		}
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
