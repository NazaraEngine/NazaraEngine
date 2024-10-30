// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz::Vk
{
	inline VkResult Sampler::CreateHelper(Device& device, const VkSamplerCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkSampler* handle)
	{
		return device.vkCreateSampler(device, createInfo, allocator, handle);
	}

	inline void Sampler::DestroyHelper(Device& device, VkSampler handle, const VkAllocationCallbacks* allocator)
	{
		return device.vkDestroySampler(device, handle, allocator);
	}
}

