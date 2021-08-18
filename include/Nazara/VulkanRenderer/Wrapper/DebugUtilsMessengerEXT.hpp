// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKDEBUGUTILSMESSENGEREXT_HPP
#define NAZARA_VULKANRENDERER_VKDEBUGUTILSMESSENGEREXT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/InstanceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class DebugUtilsMessengerEXT : public InstanceObject<DebugUtilsMessengerEXT, VkDebugUtilsMessengerEXT, VkDebugUtilsMessengerCreateInfoEXT, VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT>
		{
			friend InstanceObject;

			public:
				DebugUtilsMessengerEXT() = default;
				DebugUtilsMessengerEXT(const DebugUtilsMessengerEXT&) = delete;
				DebugUtilsMessengerEXT(DebugUtilsMessengerEXT&&) = default;
				~DebugUtilsMessengerEXT() = default;

				DebugUtilsMessengerEXT& operator=(const DebugUtilsMessengerEXT&) = delete;
				DebugUtilsMessengerEXT& operator=(DebugUtilsMessengerEXT&&) = delete;

				static inline bool IsSupported(Instance& instance);

			private:
				static inline VkResult CreateHelper(Instance& instance, const VkDebugUtilsMessengerCreateInfoEXT* createInfo, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* handle);
				static inline void DestroyHelper(Instance& instance, VkDebugUtilsMessengerEXT handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/DebugUtilsMessengerEXT.inl>

#endif // NAZARA_VULKANRENDERER_VKDEBUGUTILSMESSENGEREXT_HPP
