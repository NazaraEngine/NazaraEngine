// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	namespace Vk
	{
		inline bool ShaderModule::Create(Device& device, const UInt32* code, std::size_t size, VkShaderModuleCreateFlags flags, const VkAllocationCallbacks* allocator)
		{
			VkShaderModuleCreateInfo createInfo =
			{
				VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				nullptr,
				flags,
				size,
				code
			};

			return Create(device, createInfo, allocator);
		}

		inline VkResult ShaderModule::CreateHelper(Device& device, const VkShaderModuleCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkShaderModule* handle)
		{
			return device.vkCreateShaderModule(device, createInfo, allocator, handle);
		}

		inline void ShaderModule::DestroyHelper(Device& device, VkShaderModule handle, const VkAllocationCallbacks* allocator)
		{
			return device.vkDestroyShaderModule(device, handle, allocator);
		}
	}
}

