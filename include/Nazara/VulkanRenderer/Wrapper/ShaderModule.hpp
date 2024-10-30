// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_SHADERMODULE_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_SHADERMODULE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>

namespace Nz::Vk
{
	class ShaderModule : public DeviceObject<ShaderModule, VkShaderModule, VkShaderModuleCreateInfo, VK_OBJECT_TYPE_SHADER_MODULE>
	{
		friend DeviceObject;

		public:
			ShaderModule() = default;
			ShaderModule(const ShaderModule&) = delete;
			ShaderModule(ShaderModule&&) = default;
			~ShaderModule() = default;

			using DeviceObject::Create;
			inline bool Create(Device& device, const UInt32* code, std::size_t size, VkShaderModuleCreateFlags flags = 0, const VkAllocationCallbacks* allocator = nullptr);

			ShaderModule& operator=(const ShaderModule&) = delete;
			ShaderModule& operator=(ShaderModule&&) = delete;

		private:
			static inline VkResult CreateHelper(Device& device, const VkShaderModuleCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkShaderModule* handle);
			static inline void DestroyHelper(Device& device, VkShaderModule handle, const VkAllocationCallbacks* allocator);
	};
}

#include <Nazara/VulkanRenderer/Wrapper/ShaderModule.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_SHADERMODULE_HPP
