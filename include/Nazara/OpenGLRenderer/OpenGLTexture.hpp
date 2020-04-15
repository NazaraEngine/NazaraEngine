// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLTEXTURE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLTEXTURE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Image.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/ImageView.hpp>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLTexture : public Texture
	{
		public:
			OpenGLTexture(Vk::Device& device, const TextureInfo& params);
			OpenGLTexture(const OpenGLTexture&) = default;
			OpenGLTexture(OpenGLTexture&&) noexcept = default;
			~OpenGLTexture();

			PixelFormat GetFormat() const override;
			inline VkImage GetImage() const;
			inline VkImageView GetImageView() const;
			UInt8 GetLevelCount() const override;
			Vector3ui GetSize(UInt8 level = 0) const override;
			ImageType GetType() const override;

			bool Update(const void* ptr) override;

			OpenGLTexture& operator=(const OpenGLTexture&) = delete;
			OpenGLTexture& operator=(OpenGLTexture&&) = delete;

		private:
			static void InitForFormat(PixelFormat pixelFormat, VkImageCreateInfo& createImage, VkImageViewCreateInfo& createImageView);

			VkImage m_image;
			VmaAllocation m_allocation;
			Vk::Device& m_device;
			Vk::ImageView m_imageView;
			TextureInfo m_params;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLTexture.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLTEXTURE_HPP
