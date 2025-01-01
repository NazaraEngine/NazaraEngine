// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_SWAPCHAIN_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_SWAPCHAIN_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>
#include <Nazara/VulkanRenderer/Wrapper/ImageView.hpp>

namespace Nz::Vk
{
	class Swapchain : public DeviceObject<Swapchain, VkSwapchainKHR, VkSwapchainCreateInfoKHR, VK_OBJECT_TYPE_SWAPCHAIN_KHR>
	{
		friend DeviceObject;

		public:
			struct Image;

			Swapchain() = default;
			Swapchain(const Swapchain&) = delete;
			Swapchain(Swapchain&&) = default;
			~Swapchain() = default;

			inline bool AcquireNextImage(Nz::UInt64 timeout, VkSemaphore semaphore, VkFence fence, UInt32* imageIndex) const;

			inline bool Create(Device& device, const VkSwapchainCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator = nullptr);

			inline const Image& GetImage(UInt32 index) const;
			inline const std::vector<Image>& GetImages() const;
			inline UInt32 GetImageCount() const;

			inline bool IsSupported() const;

			Swapchain& operator=(const Swapchain&) = delete;
			Swapchain& operator=(Swapchain&&) = default;

			struct Image
			{
				VkImage image;
				ImageView view;
			};

		private:
			static inline VkResult CreateHelper(Device& device, const VkSwapchainCreateInfoKHR* createInfo, const VkAllocationCallbacks* allocator, VkSwapchainKHR* handle);
			static inline void DestroyHelper(Device& device, VkSwapchainKHR handle, const VkAllocationCallbacks* allocator);

			std::vector<Image> m_images;
	};
}

#include <Nazara/VulkanRenderer/Wrapper/Swapchain.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_SWAPCHAIN_HPP
