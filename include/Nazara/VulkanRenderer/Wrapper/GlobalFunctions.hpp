// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if !defined(NAZARA_VULKANRENDERER_GLOBAL_FUNCTION) || !defined(NAZARA_VULKANRENDERER_GLOBAL_FUNCTION_OPT)
#error You must define NAZARA_VULKANRENDERER_GLOBAL_FUNCTION and NAZARA_VULKANRENDERER_GLOBAL_FUNCTION_OPT before including this file
#endif

// Vulkan core
NAZARA_VULKANRENDERER_GLOBAL_FUNCTION(vkCreateInstance)
NAZARA_VULKANRENDERER_GLOBAL_FUNCTION(vkEnumerateInstanceExtensionProperties)
NAZARA_VULKANRENDERER_GLOBAL_FUNCTION(vkEnumerateInstanceLayerProperties)
NAZARA_VULKANRENDERER_GLOBAL_FUNCTION_OPT(vkEnumerateInstanceVersion)

#undef NAZARA_VULKANRENDERER_GLOBAL_FUNCTION
#undef NAZARA_VULKANRENDERER_GLOBAL_FUNCTION_OPT
