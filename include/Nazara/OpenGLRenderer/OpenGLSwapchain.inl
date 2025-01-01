// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <cassert>

namespace Nz
{
	inline GL::Context& OpenGLSwapchain::GetContext()
	{
		assert(m_context);
		return *m_context;
	}

	inline OpenGLDevice& OpenGLSwapchain::GetDevice()
	{
		return m_device;
	}
}
