// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanShaderBinding.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VkSampler VulkanTextureSampler::GetSampler() const
	{
		return m_sampler;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
