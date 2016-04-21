// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_LOADER_HPP
#define NAZARA_VULKAN_LOADER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Vulkan/Config.hpp>
#include <vulkan/vulkan.h>

namespace Nz
{
	class NAZARA_VULKAN_API VkLoader
	{
		public:
			VkLoader() = delete;
			~VkLoader() = delete;

			static bool Initialize();

		private:
			static DynLib s_vulkanLib;
	};
}

#define NAZARA_VULKAN_EXPORTED_FUNCTION(func) extern PFN_##func func;

NAZARA_VULKAN_EXPORTED_FUNCTION(vkGetInstanceProcAddr);

#endif // NAZARA_VULKAN_LOADER_HPP
