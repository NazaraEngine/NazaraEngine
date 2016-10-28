// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKSHADERMODULE_HPP
#define NAZARA_VULKANRENDERER_VKSHADERMODULE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class ShaderModule : public DeviceObject<ShaderModule, VkShaderModule, VkShaderModuleCreateInfo>
		{
			friend DeviceObject;

			public:
				ShaderModule() = default;
				ShaderModule(const ShaderModule&) = delete;
				ShaderModule(ShaderModule&&) = default;
				~ShaderModule() = default;

				using DeviceObject::Create;
				inline bool Create(const DeviceHandle& device, const UInt32* code, std::size_t size, VkShaderModuleCreateFlags flags = 0, const VkAllocationCallbacks* allocator = nullptr);

				ShaderModule& operator=(const ShaderModule&) = delete;
				ShaderModule& operator=(ShaderModule&&) = delete;

			private:
				static inline VkResult CreateHelper(const DeviceHandle& device, const VkShaderModuleCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkShaderModule* handle);
				static inline void DestroyHelper(const DeviceHandle& device, VkShaderModule handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/ShaderModule.inl>

#endif // NAZARA_VULKANRENDERER_VKSHADERMODULE_HPP
