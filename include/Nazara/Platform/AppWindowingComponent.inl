// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/AppWindowingComponent.hpp>
#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	template<typename ...Args>
	Window& AppWindowingComponent::CreateWindow(Args&&... args)
	{
		return *m_windows.emplace_back(std::make_unique<Window>(std::forward<Args>(args)...));
	}

	inline void AppWindowingComponent::DisableQuitOnLastWindowClosed()
	{
		return EnableQuitOnLastWindowClosed(false);
	}

	inline void AppWindowingComponent::EnableQuitOnLastWindowClosed(bool enable)
	{
		m_quitOnLastWindowClosed = enable;
	}

	inline bool AppWindowingComponent::IsQuitOnLastWindowClosedEnabled() const
	{
		return m_quitOnLastWindowClosed;
	}
}

#include <Nazara/Platform/DebugOff.hpp>
