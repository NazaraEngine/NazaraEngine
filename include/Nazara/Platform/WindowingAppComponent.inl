// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	template<typename... Args>
	Window& WindowingAppComponent::CreateWindow(Args&&... args)
	{
		return *m_windows.emplace_back(std::make_unique<Window>(std::forward<Args>(args)...));
	}

	inline WindowingAppComponent::WindowingAppComponent(ApplicationBase& app) :
	ApplicationComponent(app),
	m_quitOnLastWindowClosed(true)
	{
	}

	inline void WindowingAppComponent::DisableQuitOnLastWindowClosed()
	{
		return EnableQuitOnLastWindowClosed(false);
	}

	inline void WindowingAppComponent::EnableQuitOnLastWindowClosed(bool enable)
	{
		m_quitOnLastWindowClosed = enable;
	}

	inline bool WindowingAppComponent::IsQuitOnLastWindowClosedEnabled() const
	{
		return m_quitOnLastWindowClosed;
	}
}
