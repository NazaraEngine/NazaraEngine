// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLContextBase.hpp>
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

	inline bool EGLContextBase::HasPlatformExtension(const std::string& str) const
	{
		return m_supportedPlatformExtensions.find(str) != m_supportedPlatformExtensions.end();
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
