// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/WindowingAppComponent.hpp>
#include <Nazara/Core/ApplicationBase.hpp>

namespace Nz
{
	void WindowingAppComponent::Update(Time /*elapsedTime*/)
	{
		Window::ProcessEvents();

		for (auto it = m_windows.begin(); it != m_windows.end();)
		{
			Window& window = **it;
			if (!window.IsOpen(true))
			{
				it = m_windows.erase(it);
				if (m_quitOnLastWindowClosed && m_windows.empty())
					GetApp().Quit();
			}
			else
				++it;
		}
	}
}
