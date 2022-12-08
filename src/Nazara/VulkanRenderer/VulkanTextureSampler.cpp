// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanTextureSampler.hpp>
#include <Nazara/VulkanRenderer/VulkanDevice.hpp>
#include <stdexcept>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanTextureSampler::VulkanTextureSampler(VulkanDevice& device, TextureSamplerInfo samplerInfo)
	{
		ValidateSamplerInfo(device, samplerInfo);

		VkSamplerCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		createInfo.magFilter = ToVulkan(samplerInfo.magFilter);
		createInfo.minFilter = ToVulkan(samplerInfo.minFilter);
		createInfo.addressModeU = ToVulkan(samplerInfo.wrapModeU);
		createInfo.addressModeV = ToVulkan(samplerInfo.wrapModeV);
		createInfo.addressModeW = ToVulkan(samplerInfo.wrapModeW);
		createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		createInfo.mipmapMode = ToVulkan(samplerInfo.mipmapMode);
		createInfo.compareEnable = samplerInfo.depthCompare;
		createInfo.compareOp = ToVulkan(samplerInfo.depthComparison);

		if (samplerInfo.anisotropyLevel > 1.f)
		{
			createInfo.anisotropyEnable = VK_TRUE;
			createInfo.maxAnisotropy = samplerInfo.anisotropyLevel;
		}

		if (!m_sampler.Create(device, createInfo))
			throw std::runtime_error("Failed to create sampler: " + TranslateVulkanError(m_sampler.GetLastErrorCode()));
	}

	void VulkanTextureSampler::UpdateDebugName(std::string_view name)
	{
		return m_sampler.SetDebugName(name);
	}
}

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#endif
