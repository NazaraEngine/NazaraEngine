// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Application.hpp>

namespace Ndk
{
	bool Application::Run()
	{
		#ifndef NDK_SERVER
		bool hasAtLeastOneActiveWindow = false;

		auto it = m_windows.begin();
		while (it != m_windows.end())
		{
			Nz::Window& window = **it;

			if (!window.IsOpen(true))
			{
				it = m_windows.erase(it);
				continue;
			}

			hasAtLeastOneActiveWindow = true;

			++it;
		}
		#endif

		m_updateTime = m_updateClock.GetSeconds();
		m_updateClock.Restart();

		for (World& world : m_worlds)
			world.Update(m_updateTime);

		if (m_shouldQuit)
			return false;

		#ifndef NDK_SERVER
		if (m_exitOnClosedWindows && !hasAtLeastOneActiveWindow)
			return false;
		#endif

		return true;
	}

	Application* Application::s_application = nullptr;
}