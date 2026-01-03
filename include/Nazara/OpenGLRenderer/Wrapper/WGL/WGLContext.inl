// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz::GL
{
	inline WGLContext::WGLContext(const OpenGLDevice* device, const WGLLoader& loader) :
	Context(device),
	m_loader(loader),
	m_handle(nullptr)
	{
	}

	inline bool WGLContext::HasPlatformExtension(std::string_view str) const
	{
		return m_supportedPlatformExtensions.find(str) != m_supportedPlatformExtensions.end();
	}
}
