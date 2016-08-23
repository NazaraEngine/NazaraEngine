// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKSWAPCHAIN_HPP
#define NAZARA_VULKANRENDERER_VKSWAPCHAIN_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/VulkanRenderer/VkDeviceObject.hpp>
#include <Nazara/VulkanRenderer/VkImageView.hpp>

namespace Nz 
{
	namespace Vk
	{
		class Swapchain : public DeviceObject<Swapchain, VkSwapchainKHR, VkSwapchainCreateInfoKHR>
		{
			friend DeviceObject;

			public:
				struct Buffer;
				
				Swapchain() = default;
				Swapchain(const Swapchain&) = delete;
				Swapchain(Swapchain&&) = default;
				~Swapchain() = default;

				inline bool AcquireNextImage(Nz::UInt64 timeout, VkSemaphore semaphore, VkFence fence, UInt32* imageIndex) const;

				inline bool Create(const DeviceHandle& device, const VkSwapchainCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator = nullptr);

				inline const Buffer& GetBuffer(UInt32 index) const;
				inline const std::vector<Buffer>& GetBuffers() const;
				inline UInt32 GetBufferCount() const;

				inline bool IsSupported() const;

				Swapchain& operator=(const Swapchain&) = delete;
				Swapchain& operator=(Swapchain&&) = delete;

				struct Buffer
				{
					VkImage image;
					ImageView view;
				};

			private:
				static inline VkResult CreateHelper(const DeviceHandle& device, const VkSwapchainCreateInfoKHR* createInfo, const VkAllocationCallbacks* allocator, VkSwapchainKHR* handle);
				static inline void DestroyHelper(const DeviceHandle& device, VkSwapchainKHR handle, const VkAllocationCallbacks* allocator);

				std::vector<Buffer> m_buffers;
		};
	}
}

#include <Nazara/VulkanRenderer/VkSwapchain.inl>

#endif // NAZARA_VULKANRENDERER_VKSWAPCHAIN_HPP
