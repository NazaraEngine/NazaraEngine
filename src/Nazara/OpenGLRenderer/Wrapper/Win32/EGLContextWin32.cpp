// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Win32/EGLContextWin32.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLLoader.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	bool EGLContextWin32::Create(const ContextParams& params, const EGLContextBase* shareContext)
	{
		// It seems context sharing between pbuffer context and window context doesn't work, create an hidden window to handle this
		HWNDHandle window(::CreateWindowA("STATIC", nullptr, WS_DISABLED | WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, GetModuleHandle(nullptr), nullptr));
		if (!window)
		{
			NazaraErrorFmt("failed to create dummy window: {0}", Error::GetLastSystemError());
			return false;
		}

		::ShowWindow(window.get(), FALSE);

		WindowHandle windowHandle;
		windowHandle.type = WindowBackend::Windows;
		windowHandle.windows.window = window.get();

		if (!Create(params, windowHandle, shareContext))
			return false;

		m_ownedWindow = std::move(window);

		return true;
	}

	bool EGLContextWin32::Create(const ContextParams& params, WindowHandle window, const EGLContextBase* shareContext)
	{
		assert(window.type == WindowBackend::Windows);

		Destroy(); //< In case a previous display or surface hasn't been released

		m_params = params;

		if (!BindAPI())
			return false;

		HWND windowHandle = static_cast<HWND>(window.windows.window);
		/*HDC deviceContext = ::GetDC(windowHandle);
		if (!deviceContext)
		{
			NazaraErrorFmt("failed to retrieve window device context: {0}", Error::GetLastSystemError());
			return false;
		}

		m_display = m_loader.GetDefaultDisplay();
		if (!InitDisplay())
			return false;*/

		m_display = m_loader.GetDefaultDisplay();

		std::size_t configCount;
		std::array<EGLConfig, 0xFF> configs;
		if (!ChooseConfig(configs.data(), configs.size(), &configCount))
			return false;

		EGLint surfaceAttributes[] = {
			EGL_NONE
		};

		std::size_t configIndex = 0;
		for (; configIndex < configCount; ++configIndex)
		{
			m_surface = m_loader.eglCreateWindowSurface(m_display, configs[configIndex], windowHandle, surfaceAttributes);
			if (m_surface)
				break;
		}

		return CreateInternal(configs[configIndex], shareContext);
	}

	void EGLContextWin32::Destroy()
	{
		EGLContextBase::Destroy();
		m_ownedWindow.reset();
	}
}

#include <Nazara/Core/AntiWindows.hpp>
