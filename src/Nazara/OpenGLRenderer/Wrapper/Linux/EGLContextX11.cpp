// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Linux/EGLContextX11.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLLoader.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	bool EGLContextX11::Create(const ContextParams& params, WindowHandle window, const EGLContextBase* shareContext)
	{
		assert(window.type == WindowManager::X11);

		NazaraError("X11 is not yet supported");
		return false;
	}

	void EGLContextX11::Destroy()
	{
		EGLContextBase::Destroy();
	}
}
