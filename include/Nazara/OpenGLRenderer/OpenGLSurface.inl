// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLSurface.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline Vk::Surface& OpenGLSurface::GetSurface()
	{
		return m_surface;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
