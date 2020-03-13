// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKIMAGE_HPP
#define NAZARA_VULKANRENDERER_VKIMAGE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>

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
				static inline VkResult CreateHelper(Device& device, const VkImageCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkImage* handle);
				static inline void DestroyHelper(Device& device, VkImage handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/Image.inl>

#endif // NAZARA_VULKANRENDERER_VKIMAGE_HPP
