// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	inline WebContext::WebContext(const OpenGLDevice* device, const WebLoader& loader) :
	Context(device),
	m_loader(loader),
	m_handle(0)
	{
	}

	inline bool WebContext::HasPlatformExtension(std::string_view str) const
	{
		return m_supportedPlatformExtensions.contains(str);
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
