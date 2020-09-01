// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Linux/EGLContextWayland.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLLoader.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	bool EGLContextWayland::Create(const ContextParams& params, WindowHandle window, const EGLContextBase* shareContext)
	{
		assert(window.type == WindowManager::Wayland);

		NazaraError("Wayland is not yet supported");
		return false;
	}

	void EGLContextWayland::Destroy()
	{
		EGLContextBase::Destroy();
	}
}
