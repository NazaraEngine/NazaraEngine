// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
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
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
