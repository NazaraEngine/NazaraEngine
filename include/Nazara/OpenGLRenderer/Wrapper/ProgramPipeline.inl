// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Image.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline bool Image::BindImageMemory(VkDeviceMemory memory, VkDeviceSize offset)
		{
			m_lastErrorCode = m_device->vkBindImageMemory(*m_device, m_handle, memory, offset);
			if (m_lastErrorCode != VK_SUCCESS)
			{
				NazaraError("Failed to bind image memory: " + TranslateOpenGLError(m_lastErrorCode));
				return false;
			}

			return true;
		}

		inline VkMemoryRequirements Image::GetMemoryRequirements() const
		{
			NazaraAssert(IsValid(), "Invalid image");

			VkMemoryRequirements memoryRequirements;
			m_device->vkGetImageMemoryRequirements(*m_device, m_handle, &memoryRequirements);

			return memoryRequirements;
		}

		inline VkResult Image::CreateHelper(Device& device, const VkImageCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkImage* handle)
		{
			return device.vkCreateImage(device, createInfo, allocator, handle);
		}

		inline void Image::DestroyHelper(Device& device, VkImage handle, const VkAllocationCallbacks* allocator)
		{
			return device.vkDestroyImage(device, handle, allocator);
		}
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
