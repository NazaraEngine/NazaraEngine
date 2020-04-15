// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Sampler.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
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
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
