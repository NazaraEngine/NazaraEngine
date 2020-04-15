// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/ImageView.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline VkResult ImageView::CreateHelper(Device& device, const VkImageViewCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkImageView* handle)
		{
			return device.vkCreateImageView(device, createInfo, allocator, handle);
		}

		inline void ImageView::DestroyHelper(Device& device, VkImageView handle, const VkAllocationCallbacks* allocator)
		{
			return device.vkDestroyImageView(device, handle, allocator);
		}
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
