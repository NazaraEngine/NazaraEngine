// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	inline EGLContextBase::EGLContextBase(const OpenGLDevice* device, const EGLLoader& loader) :
	Context(device),
	m_loader(loader),
	m_display(EGL_NO_DISPLAY),
	m_surface(EGL_NO_SURFACE),
	m_handle(EGL_NO_CONTEXT),
	m_ownsDisplay(false)
	{
	}

	inline bool EGLContextBase::HasPlatformExtension(std::string_view str) const
	{
		return m_supportedPlatformExtensions.contains(str);
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
