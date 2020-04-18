// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/DummySurface.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	bool DummySurface::Create(WindowHandle handle)
	{
		m_handle = handle;
		return true;
	}

	void DummySurface::Destroy()
	{
		m_handle = nullptr;
	}
}
