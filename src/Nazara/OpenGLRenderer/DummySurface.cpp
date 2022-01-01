// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
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
		m_handle = WindowHandle{};
	}
}
