// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
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
