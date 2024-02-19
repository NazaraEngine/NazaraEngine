// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Linux/EGLContextX11.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLLoader.hpp>

namespace Nz::GL
{
	bool EGLContextX11::Create(const ContextParams& params, WindowHandle window, const EGLContextBase* shareContext)
	{
		assert(window.type == WindowBackend::X11);

		Destroy(); //< In case a previous display or surface hasn't been released

		m_params = params;

		if (!BindAPI())
			return false;

		m_display = m_loader.GetDefaultDisplay();

		std::size_t configCount;
		std::array<EGLConfig, 0xFF> configs;
		if (!ChooseConfig(configs.data(), configs.size(), &configCount))
			return false;

		::NativeWindowType winHandle = static_cast<::NativeWindowType>(window.x11.window);

		std::size_t configIndex = 0;
		for (; configIndex < configCount; ++configIndex)
		{
			m_surface = m_loader.eglCreateWindowSurface(m_display, configs[configIndex], winHandle, nullptr);
			if (m_surface)
				break;
		}

		return CreateInternal(configs[configIndex], shareContext);
	}
}
