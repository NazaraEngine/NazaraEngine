// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKIMAGEVIEW_HPP
#define NAZARA_VULKANRENDERER_VKIMAGEVIEW_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class ImageView : public DeviceObject<ImageView, VkImageView, VkImageViewCreateInfo>
		{
			friend DeviceObject;

			public:
				ImageView() = default;
				ImageView(const ImageView&) = delete;
				ImageView(ImageView&&) = default;
				~ImageView() = default;

				ImageView& operator=(const ImageView&) = delete;
				ImageView& operator=(ImageView&&) = delete;

			private:
				static inline VkResult CreateHelper(const DeviceHandle& device, const VkImageViewCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkImageView* handle);
				static inline void DestroyHelper(const DeviceHandle& device, VkImageView handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/ImageView.inl>

#endif // NAZARA_VULKANRENDERER_VKIMAGEVIEW_HPP
