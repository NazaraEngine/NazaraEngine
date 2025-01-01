// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp


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
