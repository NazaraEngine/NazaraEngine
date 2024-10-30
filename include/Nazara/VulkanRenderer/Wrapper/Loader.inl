// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz::Vk
{
	inline PFN_vkVoidFunction Loader::GetInstanceProcAddr(VkInstance instance, const char* name)
	{
		return vkGetInstanceProcAddr(instance, name);
	}
}

