// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/Loader.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline PFN_vkVoidFunction Loader::GetInstanceProcAddr(VkInstance instance, const char* name)
		{
			return vkGetInstanceProcAddr(instance, name);
		}
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
