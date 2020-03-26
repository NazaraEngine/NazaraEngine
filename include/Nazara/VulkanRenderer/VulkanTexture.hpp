// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANTEXTURE_HPP
#define NAZARA_VULKANRENDERER_VULKANTEXTURE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Image.hpp>
#include <Nazara/VulkanRenderer/Wrapper/ImageView.hpp>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanTexture : public Texture
	{
		public:
			VulkanTexture(Vk::Device& device, const TextureInfo& params);
			VulkanTexture(const VulkanTexture&) = default;
			VulkanTexture(VulkanTexture&&) noexcept = default;
			~VulkanTexture();

			PixelFormatType GetFormat() const override;
			inline VkImage GetImage() const;
			inline VkImageView GetImageView() const;
			UInt8 GetLevelCount() const override;
			Vector3ui GetSize(UInt8 level = 0) const override;
			ImageType GetType() const override;

			bool Update(const void* ptr) override;

			VulkanTexture& operator=(const VulkanTexture&) = delete;
			VulkanTexture& operator=(VulkanTexture&&) = delete;

		private:
			static void InitForFormat(PixelFormatType pixelFormat, VkImageCreateInfo& createImage, VkImageViewCreateInfo& createImageView);

			VkImage m_image;
			VmaAllocation m_allocation;
			Vk::Device& m_device;
			Vk::ImageView m_imageView;
			TextureInfo m_params;
	};
}

#include <Nazara/VulkanRenderer/VulkanTexture.inl>

#endif // NAZARA_VULKANRENDERER_VULKANTEXTURE_HPP
