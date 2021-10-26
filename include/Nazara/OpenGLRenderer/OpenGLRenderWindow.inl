// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLRenderWindow.hpp>
#include <cassert>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline GL::Context& OpenGLRenderWindow::GetContext()
	{
		assert(m_context);
		return *m_context;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
