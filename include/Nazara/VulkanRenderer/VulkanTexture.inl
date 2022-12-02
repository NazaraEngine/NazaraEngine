// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanTexture.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VkImage VulkanTexture::GetImage() const
	{
		return m_image;
	}

	inline VkImageView VulkanTexture::GetImageView() const
	{
		return m_imageView;
	}

	inline const VkImageSubresourceRange& VulkanTexture::GetSubresourceRange() const
	{
		return m_imageRange;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
