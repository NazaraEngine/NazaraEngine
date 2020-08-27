// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLTexture.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline const GL::Texture& OpenGLTexture::GetTexture() const
	{
		return m_texture;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
