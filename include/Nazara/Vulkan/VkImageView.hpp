// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKIMAGEVIEW_HPP
#define NAZARA_VULKAN_VKIMAGEVIEW_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/VkDeviceObject.hpp>

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

#include <Nazara/Vulkan/VkImageView.inl>

#endif // NAZARA_VULKAN_VKIMAGEVIEW_HPP
