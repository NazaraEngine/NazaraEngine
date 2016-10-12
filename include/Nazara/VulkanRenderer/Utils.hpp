// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILS_VULKAN_HPP
#define NAZARA_UTILS_VULKAN_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/VulkanRenderer/VkLoader.hpp>

namespace Nz
{
	NAZARA_VULKANRENDERER_API String TranslateVulkanError(VkResult code);
}

#endif // NAZARA_UTILS_VULKAN_HPP
