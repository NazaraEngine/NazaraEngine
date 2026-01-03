// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Linux/EGLContextWayland.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLLoader.hpp>

namespace Nz::GL
{
	bool EGLContextWayland::Create(const ContextParams& params, WindowHandle window, const EGLContextBase* shareContext)
	{
		assert(window.type == WindowBackend::Wayland);

		NazaraError("Wayland is not yet supported");
		return false;
	}

	void EGLContextWayland::Destroy()
	{
		EGLContextBase::Destroy();
	}
}
