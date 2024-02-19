// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANTEXTURE_HPP
#define NAZARA_VULKANRENDERER_VULKANTEXTURE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/VulkanRenderer/Export.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Image.hpp>
#include <Nazara/VulkanRenderer/Wrapper/ImageView.hpp>
#include <optional>

namespace Nz
{
	class VulkanBuffer;
	class VulkanDevice;

	namespace Vk
	{
		class CommandBuffer;
	}

	class NAZARA_VULKANRENDERER_API VulkanTexture final : public Texture
	{
		public:
			VulkanTexture(VulkanDevice& device, const TextureInfo& textureInfo);
			VulkanTexture(VulkanDevice& device, const TextureInfo& textureInfo, const void* initialData, bool buildMipmaps, unsigned int srcWidth = 0, unsigned int srcHeight = 0);
			VulkanTexture(std::shared_ptr<VulkanTexture> parentTexture, const TextureViewInfo& viewInfo);
			VulkanTexture(const VulkanTexture&) = delete;
			VulkanTexture(VulkanTexture&&) = delete;
			~VulkanTexture();

			inline VkImageSubresourceLayers BuildSubresourceLayers(UInt32 level) const;
			inline VkImageSubresourceLayers BuildSubresourceLayers(UInt32 level, UInt32 baseLayer, UInt32 layerCount) const;
			inline VkImageSubresourceRange BuildSubresourceRange(UInt32 baseLevel, UInt32 levelCount) const;
			inline VkImageSubresourceRange BuildSubresourceRange(UInt32 baseLevel, UInt32 levelCount, UInt32 baseLayer, UInt32 layerCount) const;

			bool Copy(const Texture& source, const Boxui& srcBox, const Vector3ui& dstPos) override;
			std::shared_ptr<Texture> CreateView(const TextureViewInfo& viewInfo) override;

			inline PixelFormat GetFormat() const override;
			inline VkImage GetImage() const;
			inline VkImageView GetImageView() const;
			inline UInt8 GetLevelCount() const override;
			inline VulkanTexture* GetParentTexture() const override;
			inline Vector3ui GetSize(UInt8 level = 0) const override;
			inline const VkImageSubresourceRange& GetSubresourceRange() const;
			inline const TextureInfo& GetTextureInfo() const override;
			inline ImageType GetType() const override;

			using Texture::Update;
			bool Update(const void* ptr, const Boxui& box, unsigned int srcWidth, unsigned int srcHeight, UInt8 level) override;
			bool Update(Vk::CommandBuffer& commandBuffer, std::unique_ptr<VulkanBuffer>& uploadBuffer, const void* ptr, const Boxui& box, unsigned int srcWidth, unsigned int srcHeight, UInt8 level);

			void UpdateDebugName(std::string_view name) override;

			VulkanTexture& operator=(const VulkanTexture&) = delete;
			VulkanTexture& operator=(VulkanTexture&&) = delete;

		private:
			static void InitViewForFormat(PixelFormat pixelFormat, VkImageViewCreateInfo& createImageView);

			std::optional<TextureViewInfo> m_viewInfo;
			std::shared_ptr<VulkanTexture> m_parentTexture;
			VulkanDevice& m_device;
			VkImage m_image;
			VkImageSubresourceRange m_subresourceRange;
			VmaAllocation m_allocation;
			Vk::ImageView m_imageView;
			TextureInfo m_textureInfo;
			TextureInfo m_textureViewInfo;
	};
}

#include <Nazara/VulkanRenderer/VulkanTexture.inl>

#endif // NAZARA_VULKANRENDERER_VULKANTEXTURE_HPP
