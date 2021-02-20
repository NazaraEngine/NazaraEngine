// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
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
			case ImageType_2D:       return GL::TextureTarget::Target2D;
			case ImageType_2D_Array: return GL::TextureTarget::Target2D_Array;
			case ImageType_3D:       return GL::TextureTarget::Target3D;
			case ImageType_Cubemap:  return GL::TextureTarget::Cubemap;

			case ImageType_1D:
			case ImageType_1D_Array:
			default:
				throw std::runtime_error("unsupported texture type");
		}
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
