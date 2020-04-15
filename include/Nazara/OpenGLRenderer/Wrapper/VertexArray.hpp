// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_VKIMAGEVIEW_HPP
#define NAZARA_OPENGLRENDERER_VKIMAGEVIEW_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/DeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class ImageView : public DeviceObject<ImageView, VkImageView, VkImageViewCreateInfo, VK_OBJECT_TYPE_IMAGE_VIEW>
		{
			friend DeviceObject;

			public:
				ImageView() = default;
				ImageView(const ImageView&) = delete;
				ImageView(ImageView&&) = default;
				~ImageView() = default;

				ImageView& operator=(const ImageView&) = delete;
				ImageView& operator=(ImageView&&) = delete;

			private:
				static inline VkResult CreateHelper(Device& device, const VkImageViewCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkImageView* handle);
				static inline void DestroyHelper(Device& device, VkImageView handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/OpenGLRenderer/Wrapper/ImageView.inl>

#endif // NAZARA_OPENGLRENDERER_VKIMAGEVIEW_HPP
