// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanTexture.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline PixelFormat VulkanTexture::GetFormat() const
	{
		return m_textureInfo.pixelFormat;
	}

	inline VkImage VulkanTexture::GetImage() const
	{
		return m_image;
	}

	inline VkImageView VulkanTexture::GetImageView() const
	{
		return m_imageView;
	}

	inline UInt8 VulkanTexture::GetLevelCount() const
	{
		return m_textureInfo.levelCount;
	}

	inline VulkanTexture* VulkanTexture::GetParentTexture() const
	{
		return m_parentTexture.get();
	}

	inline Vector3ui VulkanTexture::GetSize(UInt8 level) const
	{
		return Vector3ui(GetLevelSize(m_textureInfo.width, level), GetLevelSize(m_textureInfo.height, level), GetLevelSize(m_textureInfo.depth, level));
	}

	inline const VkImageSubresourceRange& VulkanTexture::GetSubresourceRange() const
	{
		return m_imageRange;
	}

	inline const TextureInfo& VulkanTexture::GetTextureInfo() const
	{
		return m_textureInfo;
	}

	inline ImageType VulkanTexture::GetType() const
	{
		return m_textureInfo.type;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
