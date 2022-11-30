// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLTexture.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline const GL::Texture& OpenGLTexture::GetTexture() const
	{
		return m_texture;
	}

	inline GL::TextureTarget OpenGLTexture::ToTextureTarget(ImageType imageType)
	{
		switch (imageType)
		{
			case ImageType::E2D:       return GL::TextureTarget::Target2D;
			case ImageType::E2D_Array: return GL::TextureTarget::Target2D_Array;
			case ImageType::E3D:       return GL::TextureTarget::Target3D;
			case ImageType::Cubemap:   return GL::TextureTarget::Cubemap;

			// OpenGL ES doesn't support 1D textures, use 2D textures with a height of 1 instead
			case ImageType::E1D:       return GL::TextureTarget::Target2D;
			case ImageType::E1D_Array: return GL::TextureTarget::Target2D_Array;
		}

		throw std::runtime_error("unsupported texture type");
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
