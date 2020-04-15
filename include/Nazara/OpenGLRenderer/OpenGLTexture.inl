// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLTexture.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline VkImage OpenGLTexture::GetImage() const
	{
		return m_image;
	}

	inline VkImageView OpenGLTexture::GetImageView() const
	{
		return m_imageView;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
