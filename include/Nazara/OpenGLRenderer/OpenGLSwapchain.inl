// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <cassert>
#include <Nazara/OpenGLRenderer/Debug.hpp>

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

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
