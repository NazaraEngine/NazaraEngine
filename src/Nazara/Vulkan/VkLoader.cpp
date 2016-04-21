// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkLoader.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
}

#define NAZARA_VULKAN_EXPORTED_FUNCTION_IMPL(func) PFN_##func func = nullptr;

NAZARA_VULKAN_EXPORTED_FUNCTION_IMPL(vkGetInstanceProcAddr)