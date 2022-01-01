// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/WGL/WGLContext.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	inline GL::WGLContext::WGLContext(const OpenGLDevice* device, const WGLLoader& loader) :
	Context(device),
	m_loader(loader),
	m_handle(nullptr)
	{
	}

	inline bool WGLContext::HasPlatformExtension(const std::string& str) const
	{
		return m_supportedPlatformExtensions.find(str) != m_supportedPlatformExtensions.end();
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
