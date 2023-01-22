// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/AppWindowingComponent.hpp>
#include <Nazara/Core/ApplicationBase.hpp>
#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	void AppWindowingComponent::Update(Time elapsedTime)
	{
		Window::ProcessEvents();

		for (auto it = m_windows.begin(); it != m_windows.end();)
		{
			Window& window = **it;
			if (!window.IsOpen(true))
				it = m_windows.erase(it);
			else
				++it;
		}

		if (m_quitOnLastWindowClosed && m_windows.empty())
			GetApp().Quit();
	}
}
