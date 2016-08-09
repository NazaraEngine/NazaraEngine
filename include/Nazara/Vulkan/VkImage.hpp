// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKIMAGE_HPP
#define NAZARA_VULKAN_VKIMAGE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/VkDeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class Image : public DeviceObject<Image, VkImage, VkImageCreateInfo>
		{
			friend DeviceObject;

			public:
				Image() = default;
				Image(const Image&) = delete;
				Image(Image&&) = default;
				~Image() = default;

				bool BindImageMemory(VkDeviceMemory memory, VkDeviceSize offset = 0);

				VkMemoryRequirements GetMemoryRequirements() const;

				Image& operator=(const Image&) = delete;
				Image& operator=(Image&&) = delete;

			private:
				static inline VkResult CreateHelper(const DeviceHandle& device, const VkImageCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkImage* handle);
				static inline void DestroyHelper(const DeviceHandle& device, VkImage handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/Vulkan/VkImage.inl>

#endif // NAZARA_VULKAN_VKIMAGE_HPP
