// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanTextureSampler.hpp>
#include <stdexcept>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanTextureSampler::VulkanTextureSampler(Vk::Device& device, TextureSamplerInfo samplerInfo)
	{
		VkSamplerCreateInfo createInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		createInfo.magFilter = ToVulkan(samplerInfo.magFilter);
		createInfo.minFilter = ToVulkan(samplerInfo.minFilter);
		createInfo.addressModeU = ToVulkan(samplerInfo.wrapModeU);
		createInfo.addressModeV = ToVulkan(samplerInfo.wrapModeV);
		createInfo.addressModeW = ToVulkan(samplerInfo.wrapModeW);
		createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		createInfo.mipmapMode = ToVulkan(samplerInfo.mipmapMode);

		if (samplerInfo.anisotropyLevel > 1.f)
		{
			createInfo.anisotropyEnable = VK_TRUE;
			createInfo.maxAnisotropy = samplerInfo.anisotropyLevel;
		}

		if (!m_sampler.Create(device, createInfo))
			throw std::runtime_error("Failed to create sampler: " + TranslateVulkanError(m_sampler.GetLastErrorCode()));
	}
}
