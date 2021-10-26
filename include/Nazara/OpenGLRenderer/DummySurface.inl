// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/DummySurface.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline WindowHandle DummySurface::GetWindowHandle() const
	{
		return m_handle;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
