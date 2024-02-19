// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

// no header guards

#ifndef NAZARA_DOCGEN

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

#endif //NAZARA_DOCGEN
