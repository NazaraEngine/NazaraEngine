// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_DEBUGREPORTCALLBACKEXT_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_DEBUGREPORTCALLBACKEXT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/InstanceObject.hpp>

namespace Nz::Vk
{
	class DebugReportCallbackEXT : public InstanceObject<DebugReportCallbackEXT, VkDebugReportCallbackEXT, VkDebugReportCallbackCreateInfoEXT, VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT>
	{
		friend InstanceObject;

		public:
			DebugReportCallbackEXT() = default;
			DebugReportCallbackEXT(const DebugReportCallbackEXT&) = delete;
			DebugReportCallbackEXT(DebugReportCallbackEXT&&) = default;
			~DebugReportCallbackEXT() = default;

			DebugReportCallbackEXT& operator=(const DebugReportCallbackEXT&) = delete;
			DebugReportCallbackEXT& operator=(DebugReportCallbackEXT&&) = delete;

			static inline bool IsSupported(Instance& instance);

		private:
			static inline VkResult CreateHelper(Instance& instance, const VkDebugReportCallbackCreateInfoEXT* createInfo, const VkAllocationCallbacks* allocator, VkDebugReportCallbackEXT* handle);
			static inline void DestroyHelper(Instance& instance, VkDebugReportCallbackEXT handle, const VkAllocationCallbacks* allocator);
	};
}

#include <Nazara/VulkanRenderer/Wrapper/DebugReportCallbackEXT.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_DEBUGREPORTCALLBACKEXT_HPP
