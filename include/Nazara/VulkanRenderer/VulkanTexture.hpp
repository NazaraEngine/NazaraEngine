// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANTEXTURE_HPP
#define NAZARA_VULKANRENDERER_VULKANTEXTURE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Image.hpp>
#include <Nazara/VulkanRenderer/Wrapper/ImageView.hpp>
#include <optional>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanTexture : public Texture
	{
		public:
			VulkanTexture(Vk::Device& device, const TextureInfo& textureInfo);
			VulkanTexture(std::shared_ptr<VulkanTexture> parentTexture, const TextureViewInfo& viewInfo);
			VulkanTexture(const VulkanTexture&) = delete;
			VulkanTexture(VulkanTexture&&) = delete;
			~VulkanTexture();

			bool Copy(const Texture& source, const Boxui& srcBox, const Vector3ui& dstPos) override;
			std::shared_ptr<Texture> CreateView(const TextureViewInfo& viewInfo) override;

			PixelFormat GetFormat() const override;
			inline VkImage GetImage() const;
			inline VkImageView GetImageView() const;
			UInt8 GetLevelCount() const override;
			VulkanTexture* GetParentTexture() const override;
			Vector3ui GetSize(UInt8 level = 0) const override;
			inline const VkImageSubresourceRange& GetSubresourceRange() const;
			ImageType GetType() const override;

			using Texture::Update;
			bool Update(const void* ptr, const Boxui& box, unsigned int srcWidth, unsigned int srcHeight, UInt8 level) override;

			void UpdateDebugName(std::string_view name) override;

			VulkanTexture& operator=(const VulkanTexture&) = delete;
			VulkanTexture& operator=(VulkanTexture&&) = delete;

		private:
			static void InitViewForFormat(PixelFormat pixelFormat, VkImageViewCreateInfo& createImageView);

			std::optional<TextureViewInfo> m_viewInfo;
			std::shared_ptr<VulkanTexture> m_parentTexture;
			VkImage m_image;
			VkImageSubresourceRange m_imageRange;
			VmaAllocation m_allocation;
			Vk::Device& m_device;
			Vk::ImageView m_imageView;
			TextureInfo m_textureInfo;
	};
}

#include <Nazara/VulkanRenderer/VulkanTexture.inl>

#endif // NAZARA_VULKANRENDERER_VULKANTEXTURE_HPP
