// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>

namespace Nz
{
	inline VkImageSubresourceLayers VulkanTexture::BuildSubresourceLayers(UInt32 level) const
	{
		return BuildSubresourceLayers(level, 0, m_textureViewInfo.layerCount);
	}

	inline VkImageSubresourceLayers VulkanTexture::BuildSubresourceLayers(UInt32 level, UInt32 baseLayer, UInt32 layerCount) const
	{
		NazaraAssertMsg(m_subresourceRange.baseMipLevel + level < m_textureInfo.levelCount, "mipmap level out of bounds");
		NazaraAssertMsg(m_subresourceRange.baseArrayLayer + baseLayer + layerCount <= m_textureInfo.layerCount, "mipmap level out of bounds");

		VkImageSubresourceLayers subresourceLayers;
		subresourceLayers.aspectMask = m_subresourceRange.aspectMask;
		subresourceLayers.mipLevel = m_subresourceRange.baseMipLevel + level;
		subresourceLayers.baseArrayLayer = baseLayer;
		subresourceLayers.layerCount = layerCount;

		return subresourceLayers;
	}

	inline VkImageSubresourceRange VulkanTexture::BuildSubresourceRange(UInt32 baseLevel, UInt32 levelCount) const
	{
		return BuildSubresourceRange(baseLevel, levelCount, 0, m_textureInfo.layerCount);
	}

	inline VkImageSubresourceRange VulkanTexture::BuildSubresourceRange(UInt32 baseLevel, UInt32 levelCount, UInt32 baseLayer, UInt32 layerCount) const
	{
		VkImageSubresourceLayers subresourceLayers = BuildSubresourceLayers(baseLevel, baseLayer, layerCount);

		NazaraAssertMsg(subresourceLayers.mipLevel + levelCount <= m_textureInfo.levelCount, "mipmap level out of bounds");

		return {
			subresourceLayers.aspectMask,
			subresourceLayers.mipLevel,
			levelCount,
			subresourceLayers.baseArrayLayer,
			subresourceLayers.layerCount
		};
	}

	inline PixelFormat VulkanTexture::GetFormat() const
	{
		return m_textureViewInfo.pixelFormat;
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
		return m_textureViewInfo.levelCount;
	}

	inline VulkanTexture* VulkanTexture::GetParentTexture() const
	{
		return m_parentTexture.get();
	}

	inline Vector3ui VulkanTexture::GetSize(UInt8 level) const
	{
		return Vector3ui(ImageUtils::GetLevelSize(m_textureViewInfo.width, level), ImageUtils::GetLevelSize(m_textureViewInfo.height, level), ImageUtils::GetLevelSize(m_textureViewInfo.depth, level));
	}

	inline const VkImageSubresourceRange& VulkanTexture::GetSubresourceRange() const
	{
		return m_subresourceRange;
	}

	inline const TextureInfo& VulkanTexture::GetTextureInfo() const
	{
		return m_textureViewInfo;
	}

	inline ImageType VulkanTexture::GetType() const
	{
		return m_textureViewInfo.type;
	}
}
